#define CFG_FANCTL
#include "gpio-abs.h"
#include "fanctl.h"
#include "conf.h"

#include <limits.h>
#include "tim.h"
#include "rcc.h"
#include "gpio.h"
#include "uart.h"
#include "exti-abs.h"
#include "syscfg.h"
#include "assert-c.h"

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

static volatile uint32_t *get_ctltim_ccr()
{
	volatile struct tim_reg *tim = cfg_fan.ctl_tim;
	switch (cfg_fan.ctl_tim_ch) {
		case TIM_CH1: return &tim->ccr1;
		case TIM_CH2: return &tim->ccr2;
		case TIM_CH3: return &tim->ccr3;
		case TIM_CH4: return &tim->ccr4;
		default: assert(0);
	}
	return NULL;
}

void setup_fanctl()
{
	volatile struct tim_reg *tim = cfg_fan.ctl_tim;
	setup_fanctl_pins();
	// Setup PWM
#ifdef CONF_L0
	if (tim == tim2)
		rcc->apb1enr.tim2en = 1;
	else
		assert(0);

	/* Assuming startup default 2 MHz clock. */
#else
#warning is startup default clock 2 MHz?
#endif
	tim->arr = 72; /* auto-reload aka period */
	tim->psc = 1; /* prescaler: (2 MHz / 25 kHz) */
	tim->egr.ug = 1;

	tim->cr1.dir = TIM_DIR_UPCNT; /* upcounter */

	volatile union tim_ccmr_ch *ch;

	switch (cfg_fan.ctl_tim_ch) {
	case TIM_CH1: ch = &tim->ccmr.ch1; break;
	case TIM_CH2: ch = &tim->ccmr.ch2; break;
	case TIM_CH3: ch = &tim->ccmr.ch3; break;
	case TIM_CH4: ch = &tim->ccmr.ch4; break;
	default: assert(0);
	}
	ch->out.ccs = TIM_CCS_OUT;
	ch->out.ocm = 6;
	ch->out.ocpe = 1;

	switch (cfg_fan.ctl_tim_ch) {
	case TIM_CH1: tim->ccer.cc1e = 1; break;
	case TIM_CH2: tim->ccer.cc2e = 1; break;
	case TIM_CH3: tim->ccer.cc3e = 1; break;
	case TIM_CH4: tim->ccer.cc4e = 1; break;
	default: assert(0);
	}
	*get_ctltim_ccr() = 40;
	tim->cr1.cen = 1; /* enable */

	// Setup RPM counter
#ifdef CONF_L0
	rcc->apb2enr.syscfgen = 1;
	syscfg->exticr4.exti12 = SYSCFG_EXTI_PA;
	exti->imr.im12 = 1;
	exti->ftsr.ft12 = 1;
	//exti->emr.em12 = 1;
	//exti->swier.swi12 = 1;
	nvic_iser[0] |= 1 << 7;
#endif
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

	volatile uint32_t *ccr = get_ctltim_ccr();
	*ccr += offset;
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

void fanctl_setspeed(uint8_t speed)
{
	// [0,255] → [0,80]
	*get_ctltim_ccr() = speed * 1000 / 3556;
	rpm_target_delta = -1;
}

unsigned int parseBase10(const char *a, int len)
{
	int i;
	for (i = 0; i < len; i++) {
		if (a[i] > '9' || a[i] < '0') {
			return UINT_MAX;
		}
	}
	unsigned int sum = 0; int ex = 1;
	for (int b = i-1; b >= 0; b--) {
		sum += (a[b] - '0') * ex;
		ex *= 10;
	}
	return sum;

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
