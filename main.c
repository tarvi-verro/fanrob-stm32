#include <stdint.h>
#include "l4-lpuart.h"
#include "l4-dma.h"
#include <stdbool.h>
#include "l4-gpio.h"
#include "l4-rcc.h"
#include "f0-spi.h"
#include "conf-l4.h"
#include "app.h"
#include "kbd.h"
#include "adc.h"
#include "uart.h"
#include "camsig.h"
#include "heater.h"

void assert(bool);

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
	rcc->ahb2enr.iop_green_en = 1;

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

int main(void)
{
	int cnt = 0;
	setup_leds();
	//alt();
#ifdef clock
	setup_clock();
#endif
	setup_uart();
	uart_send_byte('\r');
	uart_send_byte('\n');
	uart_send_byte('\n');
	while (1) {
		cnt++;
		if (cnt % 0x80000 == 0x0) {
			//io_green->bsrr.reset.pin_green = 1;
			uart_send_byte('\r');
			uart_send_byte('\n');

			struct rtc_tr time;
			clock_get(NULL, &time, NULL);

			uart_send_byte('0' + time.ht);
			uart_send_byte('0' + time.hu);

			uart_send_byte(':');
			uart_send_byte('0' + time.mnt);
			uart_send_byte('0' + time.mnu);

			uart_send_byte(':');
			uart_send_byte('0' + time.st);
			uart_send_byte('0' + time.su);


			uart_print(" rxne:");
			uart_send_byte('0' + lpuart1->isr.rxne);

			uart_print(" cndtr:");
			uart_send_byte(dma2->ch7.cndtr + 'a');

			uart_print(" re:");
			uart_send_byte(lpuart1->cr1.re + '0');

			uart_print(" noisef:");
			uart_send_byte(lpuart1->isr.nf + '0');

			uart_print(" fe:");
			uart_send_byte(lpuart1->isr.fe + '0');

			uart_print(" pe:");
			uart_send_byte(lpuart1->isr.pe + '0');

			uart_print(" rdr:");
			uart_send_byte(lpuart1->rdr.rdr);

			uart_print(" ore:");
			uart_send_byte(lpuart1->isr.ore + '0');

			uart_print(" idle:");
			uart_send_byte(lpuart1->isr.idle + '0');

			uart_print(" busy:");
			uart_send_byte(lpuart1->isr.busy + '0');

			if (lpuart1->isr.idle) {
				lpuart1->icr.idlecf = 1;
			}

			uart_send_byte('\r');
			uart_send_byte('\n');
		} else if (cnt % 0x40000 == 0x0) {
			//io_green->bsrr.set.pin_green = 1;
			uart_send_byte('\r');
			uart_send_byte('\n');
		}
		if (cnt % 0x10000 == 0x0) {
			uart_send_byte(' ');
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

