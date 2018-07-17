#pragma once
#define _GPIO_ABS_H
#include "gpio.h"
#include "regs.h"
#include "rcc.h"
#include "assert-c.h"


__attribute__ ((__packed__))  enum pin {
	PA0, PA1, PA2, PA3, PA4, PA5, PA6, PA7, PA8, PA9, PA10, PA11, PA12, PA13, PA14, PA15,
	PB0, PB1, PB2, PB3, PB4, PB5, PB6, PB7, PB8, PB9, PB10, PB11, PB12, PB13, PB14, PB15,
	PC0, PC1, PC2, PC3, PC4, PC5, PC6, PC7, PC8, PC9, PC10, PC11, PC12, PC13, PC14, PC15,
	PD0, PD1, PD2, PD3, PD4, PD5, PD6, PD7, PD8, PD9, PD10, PD11, PD12, PD13, PD14, PD15,
	PE0, PE1, PE2, PE3, PE4, PE5, PE6, PE7, PE8, PE9, PE10, PE11, PE12, PE13, PE14, PE15,
	PF0, PF1, PF2, PF3, PF4, PF5, PF6, PF7, PF8, PF9, PF10, PF11, PF12, PF13, PF14, PF15,
	PG0, PG1, PG2, PG3, PG4, PG5, PG6, PG7, PG8, PG9, PG10, PG11, PG12, PG13, PG14, PG15,
	PH0, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8, PH9, PH10, PH11, PH12, PH13, PH14, PH15,
	PI0, PI1, PI2, PI3, PI4, PI5, PI6, PI7, PI8, PI9, PI10, PI11, PI12, PI13, PI14, PI15,
	PJ0, PJ1, PJ2, PJ3, PJ4, PJ5, PJ6, PJ7, PJ8, PJ9, PJ10, PJ11, PJ12, PJ13, PJ14, PJ15,
	PK0, PK1, PK2, PK3, PK4, PK5, PK6, PK7, PK8, PK9, PK10, PK11, PK12, PK13, PK14, PK15,
	PNONE,
};

struct gpio_conf {
	enum gpio_mode mode;
	enum gpio_otype otype;
	enum gpio_ospeed ospeed;
	enum gpio_pupd pupd;
	int alt;
};

static inline volatile struct gpio_reg *gpio_pin_reg(enum pin p)
{
	assert(p != PNONE);
	switch (p / 16 + 'a') {
		case 'a':
			return gpio_reg_a;
		case 'b':
			return gpio_reg_b;
		case 'c':
			return gpio_reg_c;
		case 'd':
			return gpio_reg_d;
		case 'e':
			return gpio_reg_e;
		case 'f':
			return gpio_reg_f;
		case 'g':
			return gpio_reg_g;
		case 'h':
			return gpio_reg_h;
		case 'i':
			return gpio_reg_i;
		case 'j':
			return gpio_reg_j;
		case 'k':
			return gpio_reg_k;
		default:
			assert(1==3);
	}
	return (volatile struct gpio_reg *) 0x0;
}

static inline void gpio_configure(enum pin p, struct gpio_conf *cfg)
{
	if (p == PNONE) return;
	switch (p / 16 + 'a') {
		case 'a': rcc->ahb1enr.gpioaen = 1; break;
		case 'b': rcc->ahb1enr.gpioben = 1; break;
		case 'c': rcc->ahb1enr.gpiocen = 1; break;
		case 'd': rcc->ahb1enr.gpioden = 1; break;
		case 'e': rcc->ahb1enr.gpioeen = 1; break;
		case 'f': rcc->ahb1enr.gpiofen = 1; break;
		case 'g': rcc->ahb1enr.gpiogen = 1; break;
		case 'h': rcc->ahb1enr.gpiohen = 1; break;
		case 'i': rcc->ahb1enr.gpioien = 1; break;
		case 'j': rcc->ahb1enr.gpiojen = 1; break;
		case 'k': rcc->ahb1enr.gpioken = 1; break;
		default: assert(1==3);
	}
	volatile struct gpio_reg *r = gpio_pin_reg(p);
	int z = p % 16;
	r->moder32 &= ~(0x3 << 2*z);
	r->moder32 |= cfg->mode << (2*z);

	r->otyper32 &= ~(0x1 << z);
	r->otyper32 |= cfg->otype << z;

	r->ospeedr32 &= ~(0x3 << 2*z);
	r->ospeedr32 |= cfg->ospeed << (2*z);

	r->pupdr32 &= ~(0x3 << 2*z);
	r->pupdr32 |= cfg->pupd << (2*z);

	if (cfg->mode != GPIO_MODER_AF)
		return;

	int zl = z % 8;

	r->afr32[z/8] &= ~(0xf << 4*zl);
	r->afr32[z/8] |= cfg->alt << 4*zl;
}

static inline int gpio_read(enum pin p)
{
	assert(p != PNONE);
	uint32_t id = gpio_pin_reg(p)->idr32;
	int z = p % 16;
	return !!((1 << z) & id);
}

static inline void gpio_write(enum pin p, int value)
{
	if (p == PNONE) return;
	volatile uint32_t *bsr = &gpio_pin_reg(p)->bsrr32;
	int z = p % 16;
	*bsr = 1 << (z + 16*(!value));
}

static inline void gpio_flip(enum pin p)
{
	if (p == PNONE) return;
	volatile uint32_t *od = &gpio_pin_reg(p)->odr32;
	int z = p % 16;
	*od ^= 1 << z;
}

