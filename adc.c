
#include "conf.h"
#include "adc.h"
#include "f0-adc.h"
#include "f0-rcc.h"
#include <stdbool.h>
#include "rcc.h"
extern void assert(bool);

void setup_adc() {} // TODO: make this function useful or remove it

float get_temp()
{
	// Minimum sampling time when reading temperature 5 µs.
	// temperature sensor max startup time (t_start): 10 µs
	// adc max power-up time (t_stab): 1 µs
	// ADC_IN16 input channel -- temperature sensor
	// vrefen must be set
	// ADC_IN17 -- vrefint
	if (adc->cr.aden == 1)
		return 0.f;
	rcc->apb2enr.adc_en = 1;

	adc->cr.aden = 1;
	adc->ccr.tsen = 1;
	adc->ccr.vrefen = 1;

	sleep_busy(10*1000); /* temp sensor t_start */

	// TODO: untested and unfinished code

	while (!adc->isr.adrdy);

	adc->chselr.chsel16 = 1;

	assert(!adc->cr.addis);
	adc->cr.adstart = 1;
	while (adc->cr.adstart);

	uint16_t data = adc->dr.data;
}


