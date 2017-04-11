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
		case 'h':
			return gpio_reg_h;
		default:
			assert(1==3);
	}
	return (volatile struct gpio_reg *) 0x0;
}

static inline void gpio_configure(enum pin p, struct gpio_conf *cfg)
{
	if (p == PNONE) return;
	switch (p / 16 + 'a') {
		case 'a': rcc->ahb2enr.gpioaen = 1; break;
		case 'b': rcc->ahb2enr.gpioben = 1; break;
		case 'c': rcc->ahb2enr.gpiocen = 1; break;
		case 'h': rcc->ahb2enr.gpiohen = 1; break;
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

