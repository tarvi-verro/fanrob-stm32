#include "gpio-abs.h"
#define CFG_ASSERT
#include "assert-c.h"
#include "conf.h"


void setup_assert()
{
	struct gpio_conf lcfg = {
		.mode = GPIO_MODER_OUT,
		.otype = GPIO_OTYPER_PP,
		.ospeed = GPIO_OSPEEDR_LOW,
		.pupd = GPIO_PUPDR_NONE,
	};
	gpio_configure(cfg_assert.led, &lcfg);
}

void assert(int cnd)
{
	if (cnd)
		return;
	unsigned int z = 0;
	unsigned int a;
	while (1) {
		z++;
		a = z & 0xffff;
		if (a != 0x0)
			continue;
		gpio_flip(cfg_assert.led);
	}
}

