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
#include "decimal.h"

#ifndef CONF_L0
#error "fanctl needs configuring for given target!"
#endif

static void ic_fanctl(enum pin p);
__attribute__((section(".rodata.exti.gpio.callb"))) void (*const fanctl_exti_cb)(enum pin) = ic_fanctl;

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
	exti_configure_pin(cfg_fan.rpm, &rpm_exti, &fanctl_exti_cb);
}

uint8_t ctl_duty = 0;

void setup_fanctl()
{
	setup_tim_fast();
	fanctl_setspeed(cfg_fan.ctl_initial_duty);
	setup_fanctl_pins();
}

int rpm_target_delta = -1;
int rpm_counter = 0;
int rpm_counter_delta = 0;
static int rpm_counter_previous = 0;

void rpm_chkspeed()
{
	if (rpm_target_delta == -1)
		return;

	int offset = (rpm_counter_delta - rpm_target_delta)/3;

	if (!offset)
		return;

	int l = 1;

	if (offset > l)
		offset = l;
	else if (offset < -l)
		offset = -l;

	ctl_duty += offset;
	fanctl_setspeed(ctl_duty);
}

void rpm_collect_1hz()
{
	static unsigned skip = 0;
	skip++;
	if ((skip & 1) == 0) {
		if ((skip & 6) == 0)
			rpm_chkspeed();
		return;
	}
	int z = rpm_counter;
	rpm_counter_delta = z - rpm_counter_previous;
	rpm_counter_previous = z;
}

static void ic_fanctl(enum pin p)
{
	assert(p == cfg_fan.rpm);
	rpm_counter++;
}

void fanctl_setspeed(uint8_t duty)
{
	ctl_duty = duty;
	tim_fast_duty_set(cfg_fan.ctl_fast_ch, duty);
	rpm_target_delta = -1;
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
		uart_puts("RPM counter:");
		uart_puts_int(rpm_counter);
		uart_puts(", 1Hz delta: ");
		uart_puts_int(30*rpm_counter_delta/2);
		uart_puts("\r\n");
		break;
	case 'r':
		if (len <= 2 || cmd[1] != ' ') {
			uart_puts("Expected a speed.\r\n");
			return;
		}
		sum = parseBase10(cmd + 2, len - 2);
		if ((sum % 15) != 0) {
			uart_puts("Must be a multiple of 15!\r\n");
			return;
		}
		uart_puts("Setting by RPM: ");
		uart_puts_int(sum);
		uart_puts("\r\n");
		rpm_target_delta = sum/15;
		//tim2->ccr3 = 40;
		break;
	default:
		assert(1==2);
	}
}
