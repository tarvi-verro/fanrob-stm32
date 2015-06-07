#include <stdint.h>
#include <stdbool.h>
#include "f0-gpio.h"
#include "f0-rcc.h"
#include "f0-spi.h"
#include "conf.h"
#include "app.h"
#include "kbd.h"

void assert(bool);

#define lcd 1
#ifdef lcd
#include "lcd-com.h"
#endif

#define clock 1
#ifdef clock
#include "clock.h"
#endif

void setup_usrbtn(void)
{
	uint32_t t;
	rcc->ahbenr.iop_user_en = 1;
	rcc->apb2enr.syscfg_en = 1;

	io_user->moder.pin_user = GPIO_MODER_IN;
	io_user->pupdr.pin_user = GPIO_PUPDR_PULLDOWN;

	const int trig = 0; /* page 174 : PA0 -> EXTI0 */
	exti_int[0] |= 1 << trig; /* EXTI_IMR */
	//exti_int[1] |= 1 << trig; /* EXTI_EMR */
	/* Clear rising trigger */
	exti_int[2] &= ~(1 << trig); /*EXTI_RTSR  */
	/* Set falling trigger */
	exti_int[3] |= 1 << trig; /* EXTI_FTSR */
	/* Software trigger enable */
	//exti_int[4] |= 1 << trig; /* EXTI_SWIER */

	exti_int[5] |= 1; /* EXTI_PR */

	t = syscfg_int[2]; /* SYSCFG_EXTICR1 */
	t &= ~(15 << 0); /* page 146  */
	t |= 0;
	syscfg_int[2] = t;

	nvic_iser[0] |= 1 << 5;

}

void setup_leds(void)
{
	rcc->ahbenr.iop_blue_en = 1;
	rcc->ahbenr.iop_green_en = 1;

	io_green->moder.pin_green = GPIO_MODER_OUT;
	io_blue->moder.pin_blue = GPIO_MODER_OUT;

	io_green->ospeedr.pin_green = GPIO_OSPEEDR_LOW;
	io_blue->ospeedr.pin_blue = GPIO_OSPEEDR_LOW;

	io_green->otyper.pin_green = GPIO_OTYPER_PP;
	io_blue->otyper.pin_blue = GPIO_OTYPER_PP;

	io_green->bsrr.set.pin_green = 1;
	io_blue->bsrr.set.pin_blue = 1;
}

static int somec = 0;

void undef_interr()
{
	assert(false);
}

void nvic_exti_0_1(void)
{
	exti_int[5] |= (1); /* EXTI_PR */
	if (somec)
		return;
	io_green->odr.pin_green ^= 1;
	somec = 2;
	//*nvic_icpr |= 1; /* NVIC_ISPR */

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



int main(void)
{
	setup_leds();
	setup_usrbtn();
#ifdef clock
	setup_clock();
#endif
#ifdef lcd
	setup_lcd();
	app_push(&app_info);
#endif
	setup_kbd();

	lcd_bgset(255);

	unsigned int z = 1;
	unsigned int a;
	while (1) {
		kbd_tick();
		z++;
		if ((z & 0xff) != 0x0)
			continue;
		a = z & 0xffff;
		kbd_tick_slow();
		if (a != 0x0 && a != 0x8000)
			continue;
		if (somec) {
			somec--;
			if (!somec) {
				/* usr button event handling code */
				static int a = 0;
				a++;
				switch (a % 4) {
				case 0:
					lcd_bgset(0);
					break;
				case 1:
					lcd_bgset(16);
					break;
				case 2:
					lcd_bgset(80);
					break;
				case 3:
					lcd_bgset(255);
					break;
				}
			}
		}

		if (app_update != NULL)
			app_update();

		if (io_green->odr.pin_green)
			io_blue->odr.pin_blue ^= 1;
	}
}

