#include <stdint.h>
#include "tim.h"
#include "lpuart.h"
#include "dma.h"
#include <stdbool.h>
#include "gpio.h"
#include "rcc.h"
#include "conf.h"
#include "app.h"
#include "kbd.h"
#include "adc-c.h"
#include "uart.h"

void assert(bool);
#include "fanctl.h"

#include "clock.h"


void setup_leds(void)
{
	rcc->iop_green_rcc.iop_green_en = 1;

	io_green->moder.pin_green = GPIO_MODER_OUT;
	io_green->ospeedr.pin_green = GPIO_OSPEEDR_LOW;
	io_green->otyper.pin_green = GPIO_OTYPER_PP;
	io_green->bsrr.reset.pin_green = 1;
}

void undef_interr()
{
	assert(false);
}


void assert(bool cnd)
{
	if (cnd)
		return;
	unsigned int z = 0;
	unsigned int a;
	while (1) {
		z++;
		a = z & 0xffff;
		if (a != 0x0 && a != 0x8000)
			continue;
		io_green->odr.pin_green ^= 1;
	}
}

void (*app_update)() = NULL;


extern void cmd_check(); /* cmd.c */

int main(void)
{
	int cnt = 0;
	setup_leds();
	setup_clock();

	setup_fanctl();
	setup_uart();
	uart_puts("\r\n\nChip has been restarted!\r\n");
	fanctl_setspeed(244);
	while (1) {
		cnt++;
		cmd_check();
		if (cnt % 0x10000 == 0x0)
			; // fanctl_setspeed((cnt / 0x10000) % 256);
		if (cnt % 0x80000 == 0x0) {
			io_green->bsrr.reset.pin_green = 1;
		} else if (cnt % 0x40000 == 0x0) {
			io_green->bsrr.set.pin_green = 1;
			//uart_putc('\r');
			//uart_putc('\n');
		}
		if (cnt % 0x10000 == 0x0) {
		//	uart_putc(' ');
		}
	}
}

