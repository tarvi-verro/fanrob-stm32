#include "gpio-abs.h"
#define CFG_ASSERT
#include "assert-c.h"
#include "conf.h"

#include <stdint.h>
#include "tim.h"
#include "lpuart.h"
#include "dma.h"
#include <stdbool.h>
#include "rcc.h"
#include "app.h"
#include "kbd.h"
#include "adc-c.h"
#include "uart.h"

#include "fanctl.h"

#include "clock.h"


void undef_interr()
{
	assert(0);
}


void (*app_update)() = NULL;


extern void cmd_check(); /* cmd.c */

int main(void)
{
	int cnt = 0;
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
			gpio_write(cfg_assert.led, 0);
		} else if (cnt % 0x40000 == 0x0) {
			gpio_write(cfg_assert.led, 1);
			//uart_putc('\r');
			//uart_putc('\n');
		}
		if (cnt % 0x10000 == 0x0) {
		//	uart_putc(' ');
		}
	}
}

