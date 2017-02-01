#pragma once
#include <stdint.h>
#include <stddef.h>

enum gpio_mode {
	GPIO_MODER_IN,
	GPIO_MODER_OUT,
	GPIO_MODER_AF,
	GPIO_MODER_ANALOG,
};

struct gpio_moder {
	uint32_t pin0 : 2, pin1 : 2, pin2 : 2, pin3 : 2, pin4 : 2,
		 pin5 : 2, pin6 : 2, pin7 : 2, pin8 : 2, pin9 : 2,
		 pin10 : 2, pin11 : 2, pin12 : 2, pin13 : 2,
		 pin14 : 2, pin15 : 2;
};

enum gpio_otype {
	GPIO_OTYPER_PP,
	GPIO_OTYPER_OD,
};
struct gpio_otyper {
	uint32_t pin0 : 1, pin1 : 1, pin2 : 1, pin3 : 1, pin4 : 1, pin5 : 1,
		 pin6 : 1, pin7 : 1, pin8 : 1, pin9 : 1, pin10 : 1, pin11 : 1,
		 pin12 : 1, pin13 : 1, pin14 : 1, pin15 : 1;
	uint32_t : 16; /* reserved */
};

enum gpio_ospeed {
	GPIO_OSPEEDR_LOW,
	GPIO_OSPEEDR_MEDIUM,
	GPIO_OSPEEDR_HIGH,
	GPIO_OSPEEDR_VERYHIGH,
};
struct gpio_ospeedr {
	uint32_t pin0 : 2, pin1 : 2, pin2 : 2, pin3 : 2,
		 pin4 : 2, pin5 : 2, pin6 : 2, pin7 : 2,
		 pin8 : 2, pin9 : 2, pin10 : 2, pin11 : 2,
		 pin12 : 2, pin13 : 2, pin14 : 2, pin15 : 2;
};

enum gpio_pupd {
	GPIO_PUPDR_NONE,
	GPIO_PUPDR_PULLUP,
	GPIO_PUPDR_PULLDOWN,
	GPIO_PUPDR_RESERVED,
};
struct gpio_pupdr {
	uint32_t pin0 : 2, pin1 : 2, pin2 : 2, pin3 : 2, pin4 : 2,
		 pin5 : 2, pin6 : 2, pin7 : 2, pin8 : 2, pin9 : 2,
		 pin10 : 2, pin11 : 2, pin12 : 2, pin13 : 2,
		 pin14 : 2, pin15 : 2;
};

struct gpio_idr {
	uint32_t pin0 : 1, pin1 : 1, pin2 : 1, pin3 : 1, pin4 : 1, pin5 : 1,
		 pin6 : 1, pin7 : 1, pin8 : 1, pin9 : 1, pin10 : 1, pin11 : 1,
		 pin12 : 1, pin13 : 1, pin14 : 1, pin15 : 1;
	uint32_t : 16; /* reserved */
};

struct gpio_odr {
	uint32_t pin0 : 1, pin1 : 1, pin2 : 1, pin3 : 1, pin4 : 1, pin5 : 1,
		 pin6 : 1, pin7 : 1, pin8 : 1, pin9 : 1, pin10 : 1, pin11 : 1,
		 pin12 : 1, pin13 : 1, pin14 : 1, pin15 : 1;
	uint32_t : 16; /* reserved */
};

struct gpio_bsrr {
	struct {
		uint16_t pin0 : 1, pin1 : 1, pin2 : 1, pin3 : 1, pin4 : 1,
			 pin5 : 1, pin6 : 1, pin7 : 1, pin8 : 1, pin9 : 1,
			 pin10 : 1, pin11 : 1, pin12 : 1, pin13 : 1, pin14 : 1,
			 pin15 : 1;
	} set;
	struct {
		uint16_t pin0 : 1, pin1 : 1, pin2 : 1, pin3 : 1, pin4 : 1,
			 pin5 : 1, pin6 : 1, pin7 : 1, pin8 : 1, pin9 : 1,
			 pin10 : 1, pin11 : 1, pin12 : 1, pin13 : 1, pin14 : 1,
			 pin15 : 1;
	} reset;
};

struct gpio_lckr {
	uint32_t pin0 : 1, pin1 : 1, pin2 : 1, pin3 : 1, pin4 : 1, pin5 : 1,
		 pin6 : 1, pin7 : 1, pin8 : 1, pin9 : 1, pin10 : 1, pin11 : 1,
		 pin12 : 1, pin13 : 1, pin14 : 1, pin15 : 1;
	uint32_t lckk : 1;
	uint32_t : 15; /* reserved */
};

struct gpio_afr_64 {
	uint32_t pin0 : 4, pin1 : 4, pin2 : 4, pin3 : 4, pin4 : 4, pin5 : 4,
		 pin6 : 4, pin7 : 4;

	uint32_t pin8 : 4, pin9 : 4, pin10 : 4, pin11 : 4, pin12 : 4,
		 pin13 : 4, pin14 : 4, pin15 : 4;
};

struct gpio_brr {
	uint32_t pin0 : 1, pin1 : 1, pin2 : 1, pin3 : 1, pin4 : 1, pin5 : 1,
		 pin6 : 1, pin7 : 1, pin8 : 1, pin9 : 1, pin10 : 1, pin11 : 1,
		 pin12 : 1, pin13 : 1, pin14 : 1, pin15 : 1;
	uint32_t : 16; /* reserved */
};

struct gpio_reg {
	struct gpio_moder moder;			// 0x00
	struct gpio_otyper otyper;			// 0x04
	struct gpio_ospeedr ospeedr;			// 0x08
	struct gpio_pupdr pupdr;			// 0x0C
	struct gpio_idr idr; /* input data reg */	// 0x10
	struct gpio_odr odr; /* output data reg */	// 0x14
	struct gpio_bsrr bsrr; /* set/reset */		// 0x18
	struct gpio_lckr lckr;				// 0x1C
	struct gpio_afr_64 afr;				// 0x20
	struct gpio_brr brr;				// 0x28
};

_Static_assert (offsetof(struct gpio_reg, brr) == 0x28,
		"GPIO registers have gone bad.");
