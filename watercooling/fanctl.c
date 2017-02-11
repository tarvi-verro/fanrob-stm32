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
static void ic_fanctl(enum pin p);
__attribute__((section(".rodata.exti.gpio.callb"))) void (*const fanctl_exti_rpm_cb)(enum pin) = ic_fanctl;
__attribute__((section(".rodata.exti.gpio.callb"))) void (*const fanctl_exti_pwr_cb)(enum pin) = ic_power;

static void setup_fanctl_pins()
{
	// PWM timer
	struct gpio_conf ctlc = {
		.otype = GPIO_OTYPER_PP,
		.pupd = GPIO_PUPDR_NONE,
		.mode = GPIO_MODER_AF,
		.alt = cfg_fan.ctl_tim_af,
	};
	gpio_configure(cfg_fan.ctl, &ctlc);

	// RPM counter
	struct gpio_conf rpm_gpio = {
		.mode = GPIO_MODER_IN,
		.pupd = GPIO_PUPDR_NONE,
	};
	gpio_configure(cfg_fan.rpm, &rpm_gpio);
	struct exti_conf rpm_exti = {
		.im = 1,
		.ft = 1,
	};
	exti_configure_pin(cfg_fan.rpm, &rpm_exti, &fanctl_exti_rpm_cb);

	// Small fan output
	struct gpio_conf sfn_gpio = {
		.mode = GPIO_MODER_OUT,
		.pupd = GPIO_PUPDR_PULLDOWN,
		.otype = GPIO_OTYPER_PP,
	};
	gpio_configure(cfg_fan.pwr_sfn, &sfn_gpio);

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

uint8_t fixed_duty = 0;

int rpm_counter = 0;
int rpm_counter_delta = 0;
static int rpm_counter_previous = 0;

static uint8_t duties[] = { 60, 120, 180, 200, 220, 230, 234, 237, 240, 242, 244 };
static int duties_selected; // Initially closest to cfg_fan_ctl_initial_duty
static int duties_min_rpm = 300;

static int sfn_enabled = 1;

static enum {
	STRATEGY_DUTIES,
	STRATEGY_FIXED,
} strategy = STRATEGY_DUTIES;

void duties_select_default()
{
	int delta = 255;
	for (int i = 0; i < sizeof(duties); i++) {
		int b = cfg_fan.ctl_initial_duty - duties[i];
		if (b < 0) b = -b;
		if (b < delta) {
			delta = b;
			duties_selected = i;
		} else {
			break;
		}
	}
	tim_fast_duty_set(cfg_fan.ctl_fast_ch, cfg_fan.ctl_initial_duty);
}

void setup_fanctl()
{
	setup_tim_fast();
	tim_fast_duty_set(cfg_fan.ctl_fast_ch, cfg_fan.ctl_initial_duty);
	setup_fanctl_pins();
	duties_select_default();
}

void rpm_chkspeed_duties()
{
	int rpm = 30*rpm_counter_delta/2;
	if (rpm < duties_min_rpm) {
		duties_selected--;
		if (duties_selected < 0) duties_selected = 0;
		tim_fast_duty_set(cfg_fan.ctl_fast_ch, duties[duties_selected]);
	} else if (duties_selected < sizeof(duties) - 1) {
		duties_selected++;
		tim_fast_duty_set(cfg_fan.ctl_fast_ch, duties[duties_selected]);
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
	int z = rpm_counter;
	rpm_counter_delta = z - rpm_counter_previous;
	rpm_counter_previous = z;
	if ((skip & 2) == 2 && strategy == STRATEGY_DUTIES) {
		rpm_chkspeed_duties();
		gpio_write(cfg_fan.pwr_sfn, gpio_read(cfg_fan.pwr_in) && sfn_enabled);
	}
}

static void ic_power(enum pin p)
{
	duties_select_default();
}

static void ic_fanctl(enum pin p)
{
	// Verify that we ain't getting noise
	static int last = 0;
	struct rtc_ssr c;
	clock_get(NULL, NULL, &c);
	int b =  c.ss - last;
	if (b < 0) b = -b;
	if (b < 10) return;
	last = c.ss;

	assert(p == cfg_fan.rpm);
	rpm_counter++;
}

void fanctl_setspeed(uint8_t duty)
{
	if (strategy != STRATEGY_FIXED) {
		uart_puts("Fanctl set to fixed strategy.\r\n");
		strategy = STRATEGY_FIXED;
	}
	fixed_duty = duty;
	tim_fast_duty_set(cfg_fan.ctl_fast_ch, duty);
}

void fanctl_cmd(char *cmd, int len)
{
	switch (*cmd) {
	case 'f':
		if (len <= 2 || cmd[1] != ' ') {
			uart_puts("Expected a speed [0:255].\r\n");
			return;
		}
		int sum = parseBase10(cmd + 2, len - 2);
		if (sum > 255) {
			uart_puts("Speed must be between [0:255]!\r\n");
			return;
		}
		fanctl_setspeed(sum);
		uart_puts("Set speed ");
		uart_putc('0' + sum/100);
		uart_putc('0' + (sum/10) % 10);
		uart_putc('0' + (sum/1) % 10);
		uart_puts("\r\n");

		break;
	case 'R':
	case 'r':
		uart_puts("RPM counter: ");
		uart_puts_unsigned(rpm_counter);
		uart_puts(", 1Hz delta: ");
		int rpm = 30*rpm_counter_delta/2;
		uart_puts_unsigned(rpm);
		if (rpm > 600)
			uart_putc('+');
		uart_puts("\r\n");
		if (*cmd == 'R')
			break;
		if (strategy != STRATEGY_DUTIES) {
			uart_puts("Fanctl strategy set to duties.\r\n");
			strategy = STRATEGY_DUTIES;
			tim_fast_duty_set(cfg_fan.ctl_fast_ch, duties[duties_selected]);
		}
		uart_puts("Duties: ");
		for (int i = 0; i < sizeof(duties); i++) {
			if (i == duties_selected)
				uart_putc('*');
			uart_puts_unsigned(duties[i]);
			if (i != sizeof(duties) - 1)
				uart_puts(", ");
			else
				uart_puts("\r\n");
		}
		uart_puts("Min speed: ");
		uart_puts_unsigned(duties_min_rpm);
		uart_puts("\r\n");
		break;
	case 'V':
		sfn_enabled = !sfn_enabled;
		uart_puts("Toggled small fan.\r\n");
	case 'v':
		if (sfn_enabled)
			uart_puts("Small fan enabled.\r\n");
		else
			uart_puts("Small fan disabled.\r\n");
		break;
	default:
		assert(1==2);
	}
}
