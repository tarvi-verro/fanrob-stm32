#include <stdint.h>
#include <stddef.h>


struct gpio_moder {
	uint32_t moder0 : 2, moder1 : 2, moder2 : 2, moder3 : 2, moder4 : 2,
		 moder5 : 2, moder6 : 2, moder7 : 2, moder8 : 2, moder9 : 2,
		 moder10 : 2, moder11 : 2, moder12 : 2, moder13 : 2,
		 moder14 : 2, moder15 : 2;
};

enum {
	GPIO_OT_PP,
	GPIO_OT_OD
};
struct gpio_otyper {
	uint32_t ot0 : 1, ot1 : 1, ot2 : 1, ot3 : 1, ot4 : 1, ot5 : 1, ot6 : 1,
		 ot7 : 1, ot8 : 1, ot9 : 1, ot10 : 1, ot11 : 1, ot12 : 1,
		 ot13 : 1, ot14 : 1, ot15 : 1;
	uint32_t : 16; /* reserved */
};

enum {
	GPIO_OSPEEDR_LOW,
	GPIO_OSPEEDR_MEDIUM,
	GPIO_OSPEEDR_HIGH,
};
struct gpio_ospeedr {
	uint32_t ospeedr0 : 2, ospeedr1 : 2, ospeedr2 : 2, ospeedr3 : 2,
		 ospeedr4 : 2, ospeedr5 : 2, ospeedr6 : 2, ospeedr7 : 2,
		 ospeedr8 : 2, ospeedr9 : 2, ospeedr10 : 2, ospeedr11 : 2,
		 ospeedr12 : 2, ospeedr13 : 2, ospeedr14 : 2, ospeedr15 : 2;
};

enum {
	GPIO_PUPDR_NONE,
	GPIO_PUPDR_PULLUP,
	GPIO_PUPDR_PULLDOWN,
	GPIO_PUPDR_RESERVED,
};
struct gpio_pupdr {
	uint32_t pupdr0 : 2, pupdr1 : 2, pupdr2 : 2, pupdr3 : 2, pupdr4 : 2,
		 pupdr5 : 2, pupdr6 : 2, pupdr7 : 2, pupdr8 : 2, pupdr9 : 2,
		 pupdr10 : 2, pupdr11 : 2, pupdr12 : 2, pupdr13 : 2,
		 pupdr14 : 2, pupdr15 : 2;
};

struct gpio_idr {
	uint32_t idr0 : 1, idr1 : 1, idr2 : 1, idr3 : 1, idr4 : 1, idr5 : 1,
		 idr6 : 1, idr7 : 1, idr8 : 1, idr9 : 1, idr10 : 1, idr11 : 1,
		 idr12 : 1, idr13 : 1, idr14 : 1, idr15 : 1;
	uint32_t : 16; /* reserved */
};

struct gpio_odr {
	uint32_t odr0 : 1, odr1 : 1, odr2 : 1, odr3 : 1, odr4 : 1, odr5 : 1,
		 odr6 : 1, odr7 : 1, odr8 : 1, odr9 : 1, odr10 : 1, odr11 : 1,
		 odr12 : 1, odr13 : 1, odr14 : 1, odr15 : 1;
	uint32_t : 16; /* reserved */
};

struct gpio_bsrr {
	uint32_t bs0 : 1, bs1 : 1, bs2 : 1, bs3 : 1, bs4 : 1, bs5 : 1, bs6 : 1,
		 bs7 : 1, bs8 : 1, bs9 : 1, bs10 : 1, bs11 : 1, bs12 : 1,
		 bs13 : 1, bs14 : 1, bs15 : 1;
	uint32_t br0 : 1, br1 : 1, br2 : 1, br3 : 1, br4 : 1, br5 : 1, br6 : 1,
		 br7 : 1, br8 : 1, br9 : 1, br10 : 1, br11 : 1, br12 : 1,
		 br13 : 1, br14 : 1, br15 : 1;
};

struct gpio_lckr {
	uint32_t lck0 : 1, lck1 : 1, lck2 : 1, lck3 : 1, lck4 : 1, lck5 : 1,
		 lck6 : 1, lck7 : 1, lck8 : 1, lck9 : 1, lck10 : 1, lck11 : 1,
		 lck12 : 1, lck13 : 1, lck14 : 1, lck15 : 1;
	uint32_t lckk : 1;
	uint32_t : 15; /* reserved */
};

struct gpio_afr_64 {
	uint32_t afr0 : 4, afr1 : 4, afr2 : 4, afr3 : 4, afr4 : 4, afr5 : 4,
		 afr6 : 4, afr7 : 4;

	uint32_t afr8 : 4, afr9 : 4, afr10 : 4, afr11 : 4, afr12 : 4,
		 afr13 : 4, afr14 : 4, afr15 : 4;
};

struct gpio_brr {
	uint32_t br0 : 1, br1 : 1, br2 : 1, br3 : 1, br4 : 1, br5 : 1, br6 : 1,
		 br7 : 1, br8 : 1, br9 : 1, br10 : 1, br11 : 1, br12 : 1,
		 br13 : 1, br14 : 1, br15 : 1;
	uint32_t : 16; /* reserved */
};

struct gpio_reg {
	struct gpio_moder moder;
	struct gpio_otyper otyper;
	struct gpio_ospeedr ospeedr;
	struct gpio_pupdr pupdr;
	struct gpio_idr idr;
	struct gpio_odr odr;
	struct gpio_bsrr bsrr;
	struct gpio_lckr lckr;
	struct gpio_afr_64 afr;
	struct gpio_brr brr;
};

_Static_assert (offsetof(struct gpio_reg, brr) == 0x28,
		"Gpio registers have gone bad.");
