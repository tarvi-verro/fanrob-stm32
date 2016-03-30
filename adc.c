
#include "conf.h"
#include "adc.h"
#include "f0-adc.h"
#include "f0-rcc.h"
#include <stdbool.h>
#include "rcc.h"
extern void assert(bool);

void setup_adc() {} // TODO: make this function useful or remove it

int get_temp()
{
	// Minimum sampling time when reading temperature 5 µs.
	// temperature sensor max startup time (t_start): 10 µs
	// adc max power-up time (t_stab): 1 µs
	// ADC_IN16 input channel -- temperature sensor
	// vrefen must be set
	// ADC_IN17 -- vrefint
	if (adc->cr.aden == 1)
		return 0;

	rcc->apb2enr.adc_en = 1;

	// Calibrate ADC
	adc->cr.adcal = 1;
	while (adc->cr.adcal);

	// Enable ADC, sensor and vref.
	adc->cr.aden = 1;
	adc->ccr.tsen = 1;
	adc->ccr.vrefen = 1;

	sleep_busy(10*1000); /* temp sensor t_start */

	adc->smpr.smp = 0x7;

	while (!adc->isr.adrdy);

	adc->chselr.chsel16 = 1;

	assert(!adc->cr.addis);
	adc->cr.adstart = 1;
	while (adc->cr.adstart);
	while (!adc->isr.eoc);

	int data = adc->dr.data;

	const int t30_cal = *(uint16_t *) 0x1ffff7b8;
	const int t110_cal = *(uint16_t *) 0x1ffff7c2;

	const int vdd_calib = 330;
	const int vdd_appli = 300;

	int t = data * vdd_appli / vdd_calib - t30_cal;
	t = t * (110 - 30);
	t = t / (t110_cal - t30_cal);
	t = t + 30;

	adc->cr.addis = 1;
	while (adc->cr.aden);
	return t;
}


