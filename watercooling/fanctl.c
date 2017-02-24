#define CFG_FANCTL
#include "gpio-abs.h"
#include "tim.h"
#include "tim-preset.h"
#include "fanctl.h"
#include "conf.h"

#include "rcc-abs.h"
#include "gpio.h"
#include "uart.h"
#include "exti-abs.h"
#include "syscfg.h"
#include "assert-c.h"
#include "clock.h"
#include "decimal.h"

#ifndef CONF_L0
#error "fanctl needs configuring for given target!"
#endif

static void ic_power(enum pin p);
__attribute__((section(".rodata.exti.gpio.callb"))) void (*const fanctl_exti_pwr_cb)(enum pin) = ic_power;

static void setup_fanctl_pins()
{
	for (int i = 0; i < 4; i++) {
		// PWM timer
		struct gpio_conf ctlc = {
			.otype = GPIO_OTYPER_PP,
			.pupd = GPIO_PUPDR_NONE,
			.mode = GPIO_MODER_AF,
			.alt = cfg_fan.fans[i].ctl_tim_af,
		};
		gpio_configure(cfg_fan.fans[i].ctl, &ctlc);

		// RPM counter
		struct gpio_conf rpm_gpio = {
			.mode = GPIO_MODER_IN,
			.pupd = GPIO_PUPDR_NONE,
		};
		gpio_configure(cfg_fan.fans[i].rpm, &rpm_gpio);
	}

	// 12V Power input
	struct gpio_conf pwr_gpio = {
		.mode = GPIO_MODER_IN,
		.pupd = GPIO_PUPDR_PULLDOWN,
	};
	gpio_configure(cfg_fan.pwr_in, &pwr_gpio);
	struct exti_conf pwr_exti = {
		.im = 1,
		.rt = 1,
	};
	exti_configure_pin(cfg_fan.pwr_in, &pwr_exti, &fanctl_exti_pwr_cb);
}


static int watch_print = 0;
static int watch_target = 0; /* bitmask */
extern void (*cmd_handle_override)(char *cmd, int len); /* cmd_handler.c */
static void print_watchinfo();
static void cmd_handle_override_watch(char *cmd, int len);


uint8_t fixed_duty[4] = { 0 };

int rpm_counter[4] = { 0 };
int rpm_counter_delta[4] = { 0 };
static int rpm_counter_previous[4] = { 0 };

static int duties_select[4]; // cfg_fan_ctl_initial_duty
static int duties_target_rpm[4] = { 1395, 500, 600, 600 };
static int duties_dynamic_target[4] = { 0 };

static enum {
	STRATEGY_DUTIES, /* keep target speed */
	STRATEGY_DYNAMIC, /* keep target speed, speeds set by dynamic.c */
	STRATEGY_FIXED, /* give a constant duty for timer */
} strategy[4] = {
	[0] = STRATEGY_DYNAMIC,
	[1] = STRATEGY_FIXED,
	[2] = STRATEGY_DYNAMIC,
	[3] = STRATEGY_DYNAMIC,
};

void duties_select_default()
{
	for (int fan = 0; fan < 4; fan++) {
		duties_select[fan] = cfg_fan.fans[fan].ctl_initial_duty;
		tim_fast_duty_set(cfg_fan.fans[fan].ctl_fast_ch, cfg_fan.fans[fan].ctl_initial_duty);
	}
}

void setup_fanctl()
{
	setup_tim_fast();
	duties_select_default();
	setup_fanctl_pins();
}

static inline int duties_target(int fan)
{
	assert(strategy[fan] == STRATEGY_DYNAMIC || strategy[fan] == STRATEGY_DUTIES);
	return strategy[fan] == STRATEGY_DYNAMIC ? duties_dynamic_target[fan] : duties_target_rpm[fan];
}

void rpm_chkspeed_duties()
{
	for (int fan = 0; fan < 4; fan++) {
		if (strategy[fan] != STRATEGY_DUTIES
				&& strategy[fan] != STRATEGY_DYNAMIC)
			continue;
		int rpm = 30*rpm_counter_delta[fan]/2;

		int target_delta = rpm - duties_target(fan);
		if (target_delta < 0)
			target_delta = -target_delta;

		if (target_delta <= 15)
			continue; // On target

		int step = cfg_fan.step;
		if (target_delta >= 100) {
			int d = target_delta/100;
			step *= d*d + 1;
		}

		if (rpm < duties_target(fan)) {
			duties_select[fan] -= step;
			if (duties_select[fan] < 0)
				duties_select[fan] = 0;
			tim_fast_duty_set(cfg_fan.fans[fan].ctl_fast_ch, duties_select[fan]);
		} else {
			duties_select[fan] += step;
			if (duties_select[fan] > 255)
				duties_select[fan] = 255;
			tim_fast_duty_set(cfg_fan.fans[fan].ctl_fast_ch, duties_select[fan]);
		}
	}
}

void fanctl_rpm_measure(int cnt)
{
	static uint8_t ms[4] = { 0 };
	static uint8_t is_active[4] = { 0 };
	int shft = cnt % 8;

	for (int fan = 0; fan < 4; fan++) {
		ms[fan] &= ~(1 << shft);
		ms[fan] |= gpio_read(cfg_fan.fans[fan].rpm) << shft;

		if (ms[fan] % 0xff != 0)
			continue;
		int s = ((int) ms[fan]) / 0xff;
		if (s == is_active[fan])
			continue;
		is_active[fan] = s;
		rpm_counter[fan] += s;
	}

	if (watch_print && cmd_handle_override == cmd_handle_override_watch) {
		print_watchinfo();
		watch_print = 0;
	}
}

void rpm_collect_1hz()
{
	static unsigned skip = 0;
	skip++;
	if ((skip & 1) == 0) {
		/*
		if ((skip & 6) == 0)
			rpm_chkspeed();
		*/
		return;
	}

	for (int fan = 0; fan < 4; fan++) {
		int z = rpm_counter[fan];
		rpm_counter_delta[fan] = z - rpm_counter_previous[fan];
		rpm_counter_previous[fan] = z;
	}

	if ((skip & 2) != 2)
	       return;

	rpm_chkspeed_duties();

	watch_print=1;
}

static void ic_power(enum pin p)
{
	duties_select_default();
}

int fanctl_getspeed(int fan)
{
	assert(fan >= 0 && fan < 4);
	return 30*rpm_counter_delta[fan]/2;
}

void fanctl_settarget_dyn(int fan, unsigned rpm)
{
	assert(fan >= 0 && fan < 4);
	duties_dynamic_target[fan] = rpm;
}

unsigned fanctl_gettarget_dyn(int fan)
{
	assert(fan >= 0 && fan < 4);
	return duties_dynamic_target[fan];
}

void fanctl_setspeed(int fan, uint8_t duty)
{
	assert(fan >= 0 && fan < 4);
	if (strategy[fan] != STRATEGY_FIXED) {
		uart_puts("Fanctl set to fixed strategy.\r\n");
		strategy[fan] = STRATEGY_FIXED;
	}
	fixed_duty[fan] = duty;
	tim_fast_duty_set(cfg_fan.fans[fan].ctl_fast_ch, duty);
}

static void cmd_handle_override_watch(char *cmd, int len)
{
	uart_puts("Watching stopped.\r\n");
	cmd_handle_override = NULL;
}

static void print_rpminfo(int fan, int newl, int sc)
{
	uart_puts("RPM: ");
	int rpm = 30*rpm_counter_delta[fan]/2;
	uart_puts_unsigned(rpm);
	if (strategy[fan] == STRATEGY_FIXED) {
		uart_puts(", duty: ");
		uart_puts_unsigned(tim_fast_duty_get(cfg_fan.fans[fan].ctl_fast_ch));
	}
	if (sc) {
		uart_puts(", sig count: ");
		uart_puts_unsigned(rpm_counter[fan]);
	}
	if (newl)
		uart_puts("\r\n");
}

static void print_dutiesinfo(int fan)
{
	uart_puts("Duties (t'rpm: ");
	uart_puts_unsigned(duties_target_rpm[fan]);
	uart_puts("): ");
	uart_puts_unsigned(duties_select[fan]);
	uart_puts(" (actual: ");
	uart_puts_unsigned(tim_fast_duty_get(cfg_fan.fans[fan].ctl_fast_ch));
	uart_puts(")\r\n");
}

static void print_watchinfo()
{
	for (int fan = 0; fan < 4; fan++) {
		if (!(watch_target & (1 << fan)))
			continue;
		uart_puts("Fan ");
		uart_putc('1' + fan);
		uart_putc(' ');
		print_rpminfo(fan, 0, 0);
		uart_putc(' ');
		if (strategy[fan] == STRATEGY_DUTIES)
			print_dutiesinfo(fan);
		else
			uart_puts("\r\n");
	}
}

void fanctl_cmd(char *cmd, int len)
{
	int fan;
	switch (*cmd) {
	case 'f':
		fan = cmd[1] - '1';
		if (len < 3 || fan < 0 || fan > 3) {
			uart_puts("Try: f[1:4] [0:255]\r\n");
			return;
		}
		int sum = parseBase10(cmd + 3, len - 3);
		if (sum > 255) {
			uart_puts("Speed must be between [0:255]!\r\n");
			return;
		}
		fanctl_setspeed(fan, sum);
		uart_puts("Set speed ");
		uart_putc('0' + sum/100);
		uart_putc('0' + (sum/10) % 10);
		uart_putc('0' + (sum/1) % 10);
		uart_puts("\r\n");

		break;
	case 'w':
		watch_target = 0;
		for (int i = 1; i < len; i++) {
			fan = cmd[i] - '1';
			if (len < 2 || fan < 0 || fan > 3) {
				uart_puts("Try: ");
				uart_putc(*cmd);
				uart_puts("[1:4]..\r\n");
				return;
			}
			watch_target |= 1 << fan;
		}
		if (!watch_target)
			watch_target = 0xf;

		cmd_handle_override = cmd_handle_override_watch;
		break;
	case 'R':
	case 'r':
		fan = cmd[1] - '1';
		if (len < 2 || fan < 0 || fan > 3
				|| (len > 2 && *cmd != 'r')
				|| (len > 2 && *cmd == 'r' && cmd[2] != ' ')) {
			uart_puts("Try: ");
			uart_putc(*cmd);
			uart_puts("[1:4]");
			if (*cmd == 'r')
				uart_puts(" (dyn|[target rpm])");
			uart_puts("\r\n");
			return;
		}
		print_rpminfo(fan, 1, 1);
		if (*cmd == 'R')
			break;
		if (strategy[fan] != STRATEGY_DUTIES) {
			uart_puts("Fanctl strategy set to duties.\r\n");
			strategy[fan] = STRATEGY_DUTIES;
			tim_fast_duty_set(cfg_fan.fans[fan].ctl_fast_ch,
					duties_select[fan]);
		}
		if (len > 2 && cmd[3] >= '0' && cmd[3] <= '9') {
			int ns = parseBase10(cmd + 3, len - 3);
			duties_target_rpm[fan] = ns;
			uart_puts("Target RPM set to ");
			uart_puts_unsigned(ns);
			uart_puts("\r\n");
		} else if (len > 2) {
			if (len != 6 || cmd[3] != 'd' || cmd[4] != 'y' || cmd[5] != 'n') {
				uart_puts("Did you mean 'dyn'?\r\n");
				return;
			}
			if (duties_dynamic_target[fan] == 0) {
				uart_puts("There's not dynamic targeting for said fan.\r\n");
				return;
			}
			strategy[fan] = STRATEGY_DYNAMIC;
		}
		print_dutiesinfo(fan);
		break;
	default:
		assert(1==2);
	}
}
