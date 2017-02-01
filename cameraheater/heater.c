#include "gpio-abs.h"
#define CFG_HEAT
#include "tim-preset.h"
#include "heater.h"
#include "conf.h"

#include "gpio.h"
#include "tim.h"
#include "rcc.h"

extern void setup_tim14();

void setup_heater()
{
	/* TODO: Test the FQU13N06LTU MOSFET */
	struct gpio_conf gcfg = {
		.mode = GPIO_MODER_AF,
		.ospeed = GPIO_OSPEEDR_LOW,
		.otype = GPIO_OTYPER_PP,
		.pupd = GPIO_PUPDR_NONE,
		.alt = cfg_heat.gpio_af_tim,
	};
	gpio_configure(cfg_heat.mosfet, &gcfg);

	setup_tim_slow();
	tim_slow_duty_set(cfg_heat.mosfet_tim_slow_ch, 0);
}

void heater_set(uint8_t b)
{
	tim_slow_duty_set(cfg_heat.mosfet_tim_slow_ch, b);
}

