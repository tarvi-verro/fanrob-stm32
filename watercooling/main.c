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
#include "one-wire.h"

#include "fanctl.h"
//#include "pump.h"

#include "clock.h"


#define _MAC2STR(X) #X
#define MAC2STR(X) _MAC2STR(X)

void undef_interr()
{
	assert(0);
}


void (*app_update)() = NULL;


extern void cmd_check(); /* cmd.c */

void getclock(char *cmd, int len)
{
	uart_puts("Clockinfo:");
	char *sel[] = {
		[SW_MSI_OSC] = "msi",
		[SW_HSI16_OSC] = "hsi16",
		[SW_HSE] = "hse",
		[SW_PLL] = "pll",
	};
	char *msirange[] = {
		[MSIRANGE0_65_kHz] = "65 kHz",
		[MSIRANGE1_131_kHz] = "131 kHz",
		[MSIRANGE2_262_kHz] = "262 kHz",
		[MSIRANGE3_524_kHz] = "524 kHz",
		[MSIRANGE4_1_MHz] = "1 MHz",
		[MSIRANGE5_2_MHz] = "2 MHz",
		[MSIRANGE6_4_MHz] = "4 MHz",
	};
	uart_puts("\r\n\tsysclk:\t");
	uart_puts(sel[rcc->cfgr.sw]);
	uart_puts("\r\n\trange:\t");
	if (rcc->cfgr.sw == SW_MSI_OSC) {
		uart_puts(msirange[rcc->icscr.msirange]);
	} else {
		uart_puts("?");
	}
	uart_puts("\r\n");
}

int main(void)
{
	int cnt = 0;
	setup_clock();

	setup_fanctl();
	//setup_pump();
	setup_uart();
	setup_onewire();
	uart_puts("\r\n\nChip has been restarted!\r\n");
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

