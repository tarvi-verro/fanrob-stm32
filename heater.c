#include "f0-gpio.h"
#include "f0-tim.h"
#include "conf.h"
#include "heater.h"

extern void setup_tim3();

void setup_heater()
{
	io_heat->moder.pin_heat = GPIO_MODER_AF;
	io_heat->ospeedr.pin_heat = GPIO_OSPEEDR_MEDIUM;
	io_heat->otyper.pin_heat = GPIO_OTYPER_OD;
	io_heat->pupdr.pin_heat = GPIO_PUPDR_NONE;
	io_heat->afr.pin_heat = 0; /* tim3 ch3 */

	setup_tim3();

	tim3->ccer.cc2e = 1;
	tim3->ccr2 = 1250;
}

void heater_set(uint8_t b)
{
	tim3->ccr2 = b * 10;
}

