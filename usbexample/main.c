
#include "gpio-abs.h"
#define CFG_ASSERT
#define CFG_MAIN
#include "assert-c.h"
#include "conf.h"
#include <stdint.h>
#include <stdbool.h>
#include "usb-c.h"
#include "rcc-c.h"

void setup_clocks()
{
	// Enable HSE
	rcc->cr.hseon = 1;
	while (!rcc->cr.hserdy);

	flash_acr[0] |= 2;

	rcc->cfgr.pllsrc = RCC_PLLSRC_HSE;
	rcc->cfgr.pllmul = RCC_PLLMUL_X9;

	rcc->cr.pllon = 1;
	while (!rcc->cr.pllrdy);

	rcc->cfgr.sw = RCC_SW_PLL;
	while (rcc->cfgr.sws != RCC_SW_PLL);

	sleep_granuality_ns = 13 * sleep_substract_loop_cycles;
}

int main()
{
	setup_assert();
	setup_clocks();
	setup_usb();
	int i = 0;
	gpio_write(cfg_assert.led, 1);
	for (int j = 0; j < 6; j++) {
		unsigned sleep_period = 1000*1000*1000;
		unsigned flash_period =   50*1000*1000;

		gpio_write(cfg_assert.led, 0);
		sleep_busy(flash_period);

		gpio_write(cfg_assert.led, 1);
		sleep_busy(sleep_period-flash_period);
	}
	while (true) {
		sleep_busy(1000*1000);
		i++;
		if (i % 2000 == 0 && i % 10000 < 5000)
			gpio_flip(cfg_assert.led);
	}
}

