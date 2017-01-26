#include <stdint.h>
#include "tim.h"
#include "lpuart.h"
#include "dma.h"
#include <stdbool.h>
#include "gpio.h"
#include "rcc.h"
#include "f0-spi.h"
#include "conf.h"
#include "app.h"
#include "kbd.h"
#include "adc.h"
#include "uart.h"
#include "camsig.h"
#include "heater.h"

void assert(bool);
void setup_fanctl(); /* fanctl.c */
void fanctl_setspeed(uint8_t speed);

#define lcd 1
#ifdef lcd
#include "lcd-com.h"
#endif

#define clock 1
#ifdef clock
#include "clock.h"
#endif

#if 0
void setup_usrbtn(void)
{ // TODO: porting, a lot of it required
	uint32_t t;
	//rcc->ahbenr.iop_user_en = 1;
	rcc->apb2enr.syscfgen = 1;

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
#endif

void setup_leds(void)
{
	//rcc->ahbenr.iop_blue_en = 1;
	rcc->iop_green_rcc.iop_green_en = 1;

	io_green->moder.pin_green = GPIO_MODER_OUT;
	//io_blue->moder.pin_blue = GPIO_MODER_OUT;

	io_green->ospeedr.pin_green = GPIO_OSPEEDR_LOW;
	//io_blue->ospeedr.pin_blue = GPIO_OSPEEDR_LOW;

	io_green->otyper.pin_green = GPIO_OTYPER_PP;
	//io_blue->otyper.pin_blue = GPIO_OTYPER_PP;

	io_green->bsrr.reset.pin_green = 1;
	//io_blue->bsrr.reset.pin_blue = 1;
}

static int somec = 0;

void undef_interr()
{
	assert(false);
}

#if 0
void nvic_exti_0_1(void)
{
	exti_int[5] |= (1); /* EXTI_PR */
	if (somec)
		return;
	io_green->odr.pin_green ^= 1;
	somec = 2;
	//*nvic_icpr |= 1; /* NVIC_ISPR */

}
#endif

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


#if 0
int alt(void)
{
	//io_uart->afr.pin_uart_tx = 8;
	//io_uart->afr.pin_uart_rx = 8;

	rcc->ahb2enr.iop_uart_en = 1;


	io_uart->moder.pin_uart_tx = GPIO_MODER_OUT;
	//io_uart->moder.pin_uart_rx = GPIO_MODER_OUT;

	io_uart->ospeedr.pin_uart_tx = GPIO_OSPEEDR_LOW;
	//io_uart->ospeedr.pin_uart_rx = GPIO_OSPEEDR_HIGH;

	io_uart->otyper.pin_uart_tx = GPIO_OTYPER_PP;
	//io_uart->otyper.pin_uart_rx = GPIO_OTYPER_OD;

	//io_uart->pupdr.pin_uart_tx = GPIO_PUPDR_PULLUP;
	//io_uart->pupdr.pin_uart_rx = GPIO_PUPDR_PULLUP;

	io_uart->bsrr.reset.pin_uart_tx = 1;

	int cnt = 0;
	while (1) {
		cnt++;
		if (cnt % 0x40000 == 0x0) {
			io_green->odr.pin_green ^= 1;
			io_uart->odr.pin_uart_tx ^= 1;
			//io_green->bsrr.reset.pin_green = 1;
	//		uart_send_byte('x');
		} else if (cnt % 0x20000 == 0x0) {
			//io_green->odr.pin_green ^= 1;
			io_uart->odr.pin_uart_tx ^= 1;
			//io_green->bsrr.set.pin_green = 1;
	//		uart_send_byte('o');
		}
	}
}
#endif

extern void cmd_check(); /* cmd.c */

int main(void)
{
	int cnt = 0;
	setup_leds();
	//alt();
#ifdef clock
	setup_clock();
#endif
	setup_fanctl();
	setup_uart();
	uart_print("\r\n\nChip has been restarted!\r\n");
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
			//uart_send_byte('\r');
			//uart_send_byte('\n');
		}
		if (cnt % 0x10000 == 0x0) {
		//	uart_send_byte(' ');
		}
	}
	setup_leds();
	//setup_usrbtn();
#ifdef lcd
	setup_lcd();
	app_push(&app_info);
#endif
	setup_kbd();
	setup_camsig();
	setup_heater();
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

		if (io_green->odr.pin_green)
			io_blue->odr.pin_blue ^= 1;
	}
}

