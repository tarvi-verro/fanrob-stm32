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

// Cortex-M3 lineup seems to have a different GPIO layout, so this header will
// do its best to abstract that awkwardness away.
enum gpio_mode {
	GPIO_MODER_IN,
	GPIO_MODER_OUT,
	GPIO_MODER_AF,
	GPIO_MODER_ANALOG,
};
enum gpio_otype {
	GPIO_OTYPER_PP,
	GPIO_OTYPER_OD,
};
enum gpio_ospeed {
	GPIO_OSPEEDR_MEDIUM,
	GPIO_OSPEEDR_LOW,
	GPIO_OSPEEDR_HIGH,
};
enum gpio_pupd {
	GPIO_PUPDR_NONE,
	GPIO_PUPDR_PULLUP,
	GPIO_PUPDR_PULLDOWN,
	GPIO_PUPDR_RESERVED,
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
		default:
			assert(5);
	}
	return (volatile struct gpio_reg *) 0x0;
}

static inline void gpio_configure(enum pin p, struct gpio_conf *cfg)
{
	if (p == PNONE) return;
	switch (p / 16 + 'a') {
		case 'a': rcc->apb2enr.iopaen = 1; break;
		case 'b': rcc->apb2enr.iopben = 1; break;
		case 'c': rcc->apb2enr.iopcen = 1; break;
		default: assert(0);
	}
	volatile struct gpio_reg *r = gpio_pin_reg(p);
	int z = p % 16;

	enum gpio_crl_cnf cnf;
	enum gpio_crl_mode mode;
	if (cfg->mode == GPIO_MODER_IN) {
		mode = GPIO_MODE_INPUT;
		if (cfg->pupd != GPIO_PUPDR_NONE) {
			r->odr32 &= ~(0x1 << z);
			r->odr32 |= ((cfg->pupd == GPIO_PUPDR_PULLUP) << z);
			cnf = GPIO_CNF_IN_PUPD;
		} else {
			cnf = GPIO_CNF_IN_FLOAT;
		}
	} else if (cfg->mode == GPIO_MODER_ANALOG) {
		mode = GPIO_MODE_INPUT;
		cnf = GPIO_CNF_ANALOG;
	} else if (cfg->mode == GPIO_MODER_AF) {
		mode = cfg->ospeed + 1;
		cnf = (cfg->otype == GPIO_OTYPER_PP)
			? GPIO_CNF_OUT_AF_PP : GPIO_CNF_OUT_AF_OD;
	} else if (cfg->mode == GPIO_MODER_OUT) {
		assert(cfg->ospeed >= GPIO_OSPEEDR_LOW && cfg->ospeed <= GPIO_OSPEEDR_HIGH);
		// Because enum gpio_ospeed is crafted to match enum gpio_mode
		mode = cfg->ospeed + 1;
		cnf = (cfg->otype == GPIO_OTYPER_PP)
			? GPIO_CNF_OUT_PP : GPIO_CNF_OUT_OD;
	}

	uint32_t *crl = (uint32_t *) &r->crl;
	int zl = z % 8;

	// Write out mode
	crl[z/8] &= ~(0x3 << zl*4);
	crl[z/8] |= mode << zl*4;

	// Write out cnf
	crl[z/8] &= ~(0x3 << (zl*4 + 2));
	crl[z/8] |= cnf << (zl*4 + 2);

	if (cfg->mode != GPIO_MODER_AF)
		return;

	assert(cfg->alt == -1); // TODO: Alternative function handling
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
	volatile struct gpio_bsrr *bsr = &gpio_pin_reg(p)->bsrr;
	int z = p % 16;
	*(uint32_t *) bsr = 1 << (z + 16*(!value));
}

static inline void gpio_flip(enum pin p)
{
	if (p == PNONE) return;
	volatile struct gpio_odr *od = &gpio_pin_reg(p)->odr;
	int z = p % 16;
	*(uint32_t *) od ^= 1 << z;
}

