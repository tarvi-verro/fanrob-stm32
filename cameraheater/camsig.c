#include "gpio-abs.h"
#define CFG_CAMSIG
#include "camsig.h"
#include "conf.h"

#include "gpio.h"
#include "rcc.h"

void setup_camsig()
{
	struct gpio_conf pcfg = {
		.mode = GPIO_MODER_OUT,
		.ospeed = GPIO_OSPEEDR_LOW,
		.otype = GPIO_OTYPER_PP,
	};
	gpio_configure(cfg_camsig.cam, &pcfg);
	gpio_write(cfg_camsig.cam, 0);
}

void camsig_set(int state)
{
	gpio_write(cfg_camsig.cam, state);
}

