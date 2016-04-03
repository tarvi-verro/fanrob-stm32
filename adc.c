
#include "conf.h"
#include "adc.h"
#include "f0-adc.h"
#include "f0-rcc.h"
#include <stdbool.h>
#include "rcc.h"
extern void assert(bool);

void setup_adc()
{
	rcc->apb2enr.adc_en = 1;
}

static inline int vdda_from_raw(int raw)
{
	const int vrefint_cal = *(uint16_t *) 0x1ffff7ba;
	unsigned a = raw * 1000 * 33;
	unsigned b = vrefint_cal * 12;
	// unforunately the result is rounded down, though
	return a / b;
}

int get_vdda()
{
	if (adc->cr.aden == 1)
		return 0;

	// Calibrate ADC
	adc->cr.adcal = 1;
	while (adc->cr.adcal);

	// Enable ADC, vrefint
	adc->cr.aden = 1;
	assert(!adc->cr.adstart);
	adc->ccr.vrefen = 1;

	adc->smpr.smp = 0x7;

	while (!adc->isr.adrdy);

	adc->chselr.chsel17 = 1;
	adc->cr.adstart = 1;

	while (!adc->isr.eoc);
	int data = adc->dr.data;

	adc->chselr.chsel17 = 0;
	adc->ccr.vrefen = 0;
	adc->cr.addis = 1;
	while (adc->cr.aden);
	return vdda_from_raw(data);
}

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

	// Calibrate ADC
	adc->cr.adcal = 1;
	while (adc->cr.adcal);

	// Enable ADC, sensor and vref
	adc->cr.aden = 1;
	adc->ccr.tsen = 1;
	adc->ccr.vrefen = 1;

	sleep_busy(10*1000); /* temp sensor t_start */

	adc->smpr.smp = 0x7;
	adc->cfgr.cont = 0; // single conversion mode
	adc->cfgr.discen = 1; // trigger conversions separately

	while (!adc->isr.adrdy);

	adc->chselr.chsel16 = 1; // temp
	adc->chselr.chsel17 = 1; // vref

	assert(!adc->cr.addis);
	adc->cr.adstart = 1;
	while (adc->cr.adstart);
	while (!adc->isr.eoc);

	int data = adc->dr.data;

	assert(!adc->cr.adstart);
	adc->cr.adstart = 1;
	while (adc->cr.adstart);
	while (!adc->isr.eoc);
	int vdd_appli = vdda_from_raw(adc->dr.data);

	assert(adc->isr.eoseq); // End-of-sequence

	// Calibrated at VDDA=3.3V
	const int t30_cal = *(uint16_t *) 0x1ffff7b8;
	const int t110_cal = *(uint16_t *) 0x1ffff7c2;

	const int vdd_calib = 3300;

	int t = data * vdd_appli / vdd_calib - t30_cal; // TS_DATA - TS_CAL1
	t = t * (110 - 30);
	t = t / (t110_cal - t30_cal);
	t = t + 30;

	adc->chselr.chsel16 = 0;
	adc->chselr.chsel17 = 0;
	adc->ccr.vrefen = 0;
	adc->ccr.tsen = 0;
	adc->cr.addis = 1;
	while (adc->cr.aden);
	return t;
}


