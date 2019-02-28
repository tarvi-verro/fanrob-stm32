#pragma once
#include <stdint.h>
#include <stddef.h>

enum gpio_crl_cnf {
	GPIO_CNF_ANALOG,
	GPIO_CNF_IN_FLOAT,
	GPIO_CNF_IN_PUPD,
	GPIO_CNF_RESERVED,

	// I wish GDB showed the following options aswell, oh well
	GPIO_CNF_OUT_PP = 0,
	GPIO_CNF_OUT_OD,
	GPIO_CNF_OUT_AF_PP,
	GPIO_CNF_OUT_AF_OD,
};
enum gpio_crl_mode {
	GPIO_MODE_INPUT,
	GPIO_MODE_OUT_10MHZ,
	GPIO_MODE_OUT_2MHZ,
	GPIO_MODE_OUT_50MHZ,
};
struct gpio_crl {
	enum gpio_crl_mode mode0 : 2; enum gpio_crl_cnf cnf0 : 2;
	enum gpio_crl_mode mode1 : 2; enum gpio_crl_cnf cnf1 : 2;
	enum gpio_crl_mode mode2 : 2; enum gpio_crl_cnf cnf2 : 2;
	enum gpio_crl_mode mode3 : 2; enum gpio_crl_cnf cnf3 : 2;
	enum gpio_crl_mode mode4 : 2; enum gpio_crl_cnf cnf4 : 2;
	enum gpio_crl_mode mode5 : 2; enum gpio_crl_cnf cnf5 : 2;
	enum gpio_crl_mode mode6 : 2; enum gpio_crl_cnf cnf6 : 2;
	enum gpio_crl_mode mode7 : 2; enum gpio_crl_cnf cnf7 : 2;
	enum gpio_crl_mode mode8 : 2; enum gpio_crl_cnf cnf8 : 2;
	enum gpio_crl_mode mode9 : 2; enum gpio_crl_cnf cnf9 : 2;
	enum gpio_crl_mode mode10 : 2; enum gpio_crl_cnf cnf10 : 2;
	enum gpio_crl_mode mode11 : 2; enum gpio_crl_cnf cnf11 : 2;
	enum gpio_crl_mode mode12 : 2; enum gpio_crl_cnf cnf12 : 2;
	enum gpio_crl_mode mode13 : 2; enum gpio_crl_cnf cnf13 : 2;
	enum gpio_crl_mode mode14 : 2; enum gpio_crl_cnf cnf14 : 2;
	enum gpio_crl_mode mode15 : 2; enum gpio_crl_cnf cnf15 : 2;
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

struct gpio_brr {
	uint32_t pin0 : 1, pin1 : 1, pin2 : 1, pin3 : 1, pin4 : 1, pin5 : 1,
		 pin6 : 1, pin7 : 1, pin8 : 1, pin9 : 1, pin10 : 1, pin11 : 1,
		 pin12 : 1, pin13 : 1, pin14 : 1, pin15 : 1;
	uint32_t : 16; /* reserved */
};

struct gpio_lckr {
	uint32_t pin0 : 1, pin1 : 1, pin2 : 1, pin3 : 1, pin4 : 1, pin5 : 1,
		 pin6 : 1, pin7 : 1, pin8 : 1, pin9 : 1, pin10 : 1, pin11 : 1,
		 pin12 : 1, pin13 : 1, pin14 : 1, pin15 : 1;
	uint32_t lckk : 1;
	uint32_t : 15; /* reserved */
};


struct gpio_reg {
	struct gpio_crl crl;
	union {
		struct gpio_idr idr; /* input data reg */
		uint32_t idr32;
	};
	union {
		struct gpio_odr odr; /* output data reg */
		uint32_t odr32;
	};
	struct gpio_bsrr bsrr; /* set/reset */
	struct gpio_brr brr;
	struct gpio_lckr lckr;
};

_Static_assert (offsetof(struct gpio_reg, lckr) == 0x18,
		"GPIO registers turned out the wrong size.");
