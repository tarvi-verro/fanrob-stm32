#include "gpio-abs.h"
#define CFG_ASSERT
#define CFG_MAIN
#include "assert-c.h"
#include "conf.h"
#include <stdint.h>
#include "tim.h"
#include "dma.h"
#include "exti.h"
#include "rcc.h"
#include "spi.h"
#include "app.h"
#include "kbd.h"
#include "adc-c.h"
#include "uart.h"
#include "camsig.h"
#include "heater.h"


#define lcd 1
#ifdef lcd
#include "lcd-com.h"
#endif

#define clock 1
#ifdef clock
#include "clock.h"
#endif

#include "exti-abs.h"

static void ic_user(enum pin p);

__attribute__((section(".rodata.exti.gpio.callb"))) void (*const exti_callb_mainusr)(enum pin) = ic_user;

void setup_usrbtn(void)
{
	struct gpio_conf gcfg = {
		.mode = GPIO_MODER_IN,
		.pupd = GPIO_PUPDR_PULLDOWN,
	};
	gpio_configure(cfg_main.user, &gcfg);
	struct exti_conf ecfg = {
		.im = 1,
		.ft = 1,
	};
	exti_configure_pin(cfg_main.user, &ecfg, &exti_callb_mainusr);
}

void setup_leds(void)
{
	struct gpio_conf gcfg = {
		.mode = GPIO_MODER_OUT,
		.ospeed = GPIO_OSPEEDR_LOW,
		.otype = GPIO_OTYPER_PP,
	};
	gpio_configure(cfg_main.blue, &gcfg);
	gpio_write(cfg_main.blue, 0);
}

static int somec = 0;

void undef_interr()
{
	assert(0);
}

static void ic_user(enum pin p)
{
	if (somec)
		return;
	gpio_flip(cfg_assert.led);
	somec = 2;
}

void (*app_update)() = NULL;


extern void cmd_check(); /* cmd.c */

int main(void)
{
	//setup_leds();
	setup_assert();
	setup_heater();
	//setup_usrbtn();
#ifdef clock
	setup_clock();
#endif
#ifdef lcd
	setup_lcd();
	app_push(&app_info);
#endif
	setup_kbd();
	setup_camsig();
	setup_adc();

	unsigned int z = 1;
	unsigned int a;
	while (1) {
		kbd_tick(); /* key changes */
		z++;
		if ((z & 0xff) != 0x0)
			continue;
		a = z & 0xffff;
		kbd_tick_slow();
		if ((a & 0xfff) == 0x0)
			gpio_write(cfg_assert.led, !(((a & 0xf000) >> 12) & 7));

		if (a != 0x0 && a != 0x8000)
			continue;

		if (somec) {
			somec--;
			if (!somec) {
				/* usr button event handling code */
			}
		}

		if (app_update != NULL)
			app_update();
	}
}

