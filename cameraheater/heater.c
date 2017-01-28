#include "conf.h"

#ifndef CONF_F0
#warning heater not supported on target
void setup_heater() {}
void heater_set(uint8_t b) {}
#else

#include "gpio.h"
#include "tim.h"
#include "rcc.h"
#include "heater.h"

extern void setup_tim14();

void setup_heater()
{
	/* FW232 mosfet */
	rcc->ahbenr.iop_heat_en = 1;
	io_heat->moder.pin_heat = GPIO_MODER_AF;
	io_heat->ospeedr.pin_heat = GPIO_OSPEEDR_HIGH;
	io_heat->otyper.pin_heat = GPIO_OTYPER_PP;
	io_heat->pupdr.pin_heat = GPIO_PUPDR_NONE;
	io_heat->afr.pin_heat = 0; /* tim14 ch1, AF0 */

	setup_tim14();

	tim14->ccer.cc1e = 1;
	tim14->ccr1 = 0;
}

void heater_set(uint8_t b)
{
	tim14->ccr1 = (b) * 10;
}

#endif
