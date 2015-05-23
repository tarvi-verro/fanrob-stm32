#include <stdint.h>
#include <stdbool.h>
#include "f0-gpio.h"
#include "f0-rcc.h"
#include "f0-spi.h"
#include "conf.h"

void assert(bool);

extern void lcd_send(bool data_mode /* false:command mode */,
		const uint8_t *a, int l);

extern void setup_lcd(void);
extern void lcd_tick();
extern void lcd_switch();

void setup_usrbtn(void)
{
	uint32_t t;
	rcc->ahbenr.iop_user_en = 1;
	rcc->apb2enr.syscfg_en = 1;

	io_user->moder.pin_user = GPIO_MODER_IN;
	io_user->pupdr.pin_user = GPIO_PUPDR_PULLDOWN;

	const int trig = 0; /* page 174 : PA0 -> EXTI0 */
	exti[0] |= 1 << trig; /* EXTI_IMR */
	//exti[1] |= 1 << trig; /* EXTI_EMR */
	/* Clear rising trigger */
	exti[2] &= ~(1 << trig); /*EXTI_RTSR  */
	/* Set falling trigger */
	exti[3] |= 1 << trig; /* EXTI_FTSR */
	/* Software trigger enable */
	//exti[4] |= 1 << trig; /* EXTI_SWIER */

	exti[5] &= ~(1); /* EXTI_PR */

	t = syscfg[2]; /* SYSCFG_EXTICR1 */
	t &= ~(15 << 0); /* page 146  */
	t |= 0;
	syscfg[2] = t;

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

void nvic_exti_0_1(void)
{
	exti[5] |= (1); /* EXTI_PR */
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

int main(void)
{
	setup_leds();
	setup_usrbtn();
	setup_lcd();

	unsigned int z = 1;
	unsigned int a;
	while (1) {
		z++;
		a = z & 0xffff;
		if (a != 0x0 && a != 0x8000)
			continue;
		if (somec) {
			somec--;
			if (!somec) {
				lcd_switch();
			}
		}
		lcd_tick();
		if (io_green->odr.pin_green)
			io_blue->odr.pin_blue ^= 1;
	}
}

