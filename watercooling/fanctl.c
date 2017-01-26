#include "conf.h"
#include <limits.h>
#include "tim.h"
#include "rcc.h"
#include "gpio.h"
#include "uart.h"
#include "exti.h"
#include "syscfg.h"
#include <stdbool.h>
void assert(bool);

#ifndef CONF_L0
#error "fanctl needs configuring for given target!"
#endif

static void setup_fanctl_pins()
{
	// PWM timer
	rcc->iop_fanctl_rcc.iop_fanctl_en = 1;
	io_fanctl->moder.pin_fan1_ctl = GPIO_MODER_AF;
#ifdef CONF_L0
	io_fanctl->afr.pin_fan1_ctl = 5; /* AF5 → TIM2_CH3 */
#endif

	io_fanctl->otyper.pin_fan1_ctl = GPIO_OTYPER_PP;
	io_fanctl->pupdr.pin_fan1_ctl = GPIO_PUPDR_NONE;

	// RPM counter
	rcc->iop_rpm_rcc.iop_rpm_en = 1;
	io_rpm->moder.pin_rpm = GPIO_MODER_IN;
	io_rpm->pupdr.pin_rpm = GPIO_PUPDR_NONE;
}

void setup_fanctl()
{
	setup_fanctl_pins();
	// Setup PWM
#ifdef CONF_L0
	rcc->apb1enr.tim2en = 1;
#endif
	/* Assuming startup default 2 MHz clock. */
	tim2->arr = 72; /* auto-reload aka period */
	tim2->psc = 1; /* prescaler: (2 MHz / 25 kHz) */
	tim2->egr.ug = 1;

	tim2->cr1.dir = TIM_DIR_UPCNT; /* upcounter */

	tim2->ccmr.ch3.out.ccs = TIM_CCS_OUT;
	tim2->ccmr.ch3.out.ocm = 6;
	tim2->ccmr.ch3.out.ocpe = 1;

	tim2->ccer.cc3e = 1;
	tim2->ccr3 = 40;
	tim2->cr1.cen = 1; /* enable */

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

	tim2->ccr3 += offset;
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

void rpm_interr()
{
	rpm_counter++;
	exti->pr.pif12 = 1;
}

void fanctl_setspeed(uint8_t speed)
{
	// [0,255] → [0,80]
	tim2->ccr3 = speed * 1000 / 3556;
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
			uart_print("Expected a speed [0:255].\r\n");
			return;
		}
		int sum = parseBase10(cmd + 2, len - 2);
		if (sum > 255) {
			uart_print("Speed must be between [0:255]!\r\n");
			return;
		}
		fanctl_setspeed(sum);
		uart_print("Set speed ");
		uart_send_byte('0' + sum/100);
		uart_send_byte('0' + (sum/10) % 10);
		uart_send_byte('0' + (sum/1) % 10);
		uart_print("\r\n");

		break;
	case 'R':
		uart_print("RPM counter:");
		uart_print_int(rpm_counter);
		uart_print(", 1Hz delta: ");
		uart_print_int(30*rpm_counter_delta/2);
		uart_print("\r\n");
		break;
	case 'r':
		if (len <= 2 || cmd[1] != ' ') {
			uart_print("Expected a speed.\r\n");
			return;
		}
		sum = parseBase10(cmd + 2, len - 2);
		if ((sum % 15) != 0) {
			uart_print("Must be a multiple of 15!\r\n");
			return;
		}
		uart_print("Setting by RPM: ");
		uart_print_int(sum);
		uart_print("\r\n");
		rpm_target_delta = sum/15;
		//tim2->ccr3 = 40;
		break;
	default:
		assert(1==2);
	}
}
