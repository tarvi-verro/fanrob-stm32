#include "gpio-abs.h"
#include "exti-abs.h"
#define CFG_ONEWIRE
#include "one-wire.h"
#include "usart.h"

void ic_usart_ow()
{
	exti_pending_clear(cfg_ow.extiln);
}

static void discover()
{
	// Set to 9600 baud
	// Send 0xF0
	//  > receive 0x10 to 0x90 if device is present
}

void setup_onewire()
{
	struct gpio_conf gcfg = {
		.mode = GPIO_MODER_AF,
		.otype = GPIO_OTYPER_OD,
		.pupd = GPIO_PUPDR_PULLUP,
		.ospeed = GPIO_OSPEEDR_HIGH,
		.alt = cfg_ow.tx_af,
	};
	gpio_configure(cfg_ow.tx, &gcfg);

	struct gpio_conf gcfg_pwr = {
		.mode = GPIO_MODER_OUT,
		.otype = GPIO_OTYPER_PP,
		.ospeed = GPIO_OSPEEDR_MEDIUM,
	};
	gpio_configure(cfg_ow.power, &gcfg_pwr);

	volatile struct usart_reg *usart = cfg_ow.usart;

	struct usart_cr1 cr1 = {
		.te = 1,
		.re = 1,
		.m0 = 0, // 8bit data
		.m1 = 0, // ⋮
		.rxneie = 1,
	};
	usart->cr1 = cr1;

	struct usart_cr2 cr2 = {
		.stop = 0, // 1 stop bit
	};
	usart->cr2 = cr2;

	struct usart_cr3 cr3 = {
		.hdsel = 1, // half-duplex single-wire
	};
	usart->cr3 = cr3;

	// Set up interrupt over exti
	struct exti_conf ecfg = {
		.im = 1,
	};
	exti_configure_line(cfg_ow.extiln, &ecfg);

	usart->cr1.ue = 1;
}
