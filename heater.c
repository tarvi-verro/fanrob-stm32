#include "f0-gpio.h"
#include "f0-tim.h"
#include "conf.h"
#include "heater.h"

extern void setup_tim1();

void setup_heater()
{
	io_heat->moder.pin_heat = GPIO_MODER_AF;
	io_heat->ospeedr.pin_heat = GPIO_OSPEEDR_HIGH;
	io_heat->otyper.pin_heat = GPIO_OTYPER_OD;
	io_heat->pupdr.pin_heat = GPIO_PUPDR_NONE;
	io_heat->afr.pin_heat = 2; /* tim1 ch4, AF2 */

	setup_tim1();

	tim1->ccer.cc4e = 1;
	tim1->ccr4 = 2550;
}

void heater_set(uint8_t b)
{
	tim1->ccr4 = (255 - b) * 10;
}

