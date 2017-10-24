#include "conf.h"

#if !defined (CONF_F0) && !defined (CONF_L4)
#warning adc not supported on target
void setup_adc() {}
int get_temp() { return 0; }
int get_vdda() { return 0; }
#else

#include "adc-c.h"
#include "adc.h"
#include "rcc.h"
#include <stdbool.h>
#include "rcc-c.h"
extern void assert(bool);

void setup_adc()
{
#ifdef CONF_F0
	rcc->apb2enr.adcen = 1;
#elif defined (CONF_L4)
	rcc->ccipr.adcsel = 0x3;
	rcc->ahb2enr.adcen = 1;
#endif
}

static inline int vdda_from_raw(int raw)
{
#ifdef CONF_L4
	const int vrefint_cal = *(uint16_t *) 0x1fff75aa;
	unsigned a = raw * 1000 * 30;
	unsigned b = vrefint_cal * 12;
#else
	const int vrefint_cal = *(uint16_t *) 0x1ffff7ba;
	unsigned a = raw * 1000 * 33;
	unsigned b = vrefint_cal * 12;
#endif
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
#ifdef CONF_L4
	adc->ccr.chsel17 = 1;
#else
	adc->ccr.tsen = 1;
#endif
	adc->ccr.vrefen = 1;

	sleep_busy(10*1000); /* temp sensor t_start */

#ifdef CONF_L4
	adc->smpr2.smp16 = 0x7;
	adc->smpr2.smp17 = 0x7;
#else
	adc->smpr.smp = 0x7;
#endif
	adc->cfgr.cont = 0; // single conversion mode
	adc->cfgr.discen = 1; // trigger conversions separately

	while (!adc->isr.adrdy);

#ifdef CONF_F0
	adc->chselr.chsel16 = 1; // temp
	adc->chselr.chsel17 = 1; // vref
#endif

	assert(!adc->cr.addis);
	adc->cr.adstart = 1;
	while (adc->cr.adstart);
	while (!adc->isr.eoc);

	int data = adc->dr.data;

	assert(!adc->cr.adstart);
	adc->cr.adstart = 1;
	while (adc->cr.adstart);
	while (!adc->isr.eoc);
	int data2 = adc->dr.data;

#ifdef CONF_F0
	assert(adc->isr.eoseq); // End-of-sequence
#elif defined (CONF_L4)
	assert(adc->isr.eos); // End-of-regular-conversion
#endif

#ifdef CONF_L4
	const int t30_cal = *(uint16_t *) 0x1fff75a8;
	const int t130_cal = *(uint16_t *) 0x1fff75ca;

	const int vdd_calib = 3000;

	int vdd_appli = vdda_from_raw(data2);
	int t = data * vdd_appli / vdd_calib - t30_cal; // TS_DATA - TS_CAL1
	t = t * (130 - 30);
	t = t / (t130_cal - t30_cal);
	t = t + 30;
#else
	// Calibrated at VDDA=3.3V
	const int t30_cal = *(uint16_t *) 0x1ffff7b8;
	const int t110_cal = *(uint16_t *) 0x1ffff7c2;

	const int vdd_calib = 3300;

	int vdd_appli = vdda_from_raw(data2);
	int t = data * vdd_appli / vdd_calib - t30_cal; // TS_DATA - TS_CAL1
	t = t * (110 - 30);
	t = t / (t110_cal - t30_cal);
	t = t + 30;
#endif


#ifdef CONF_F0
	adc->chselr.chsel16 = 0; // temp
	adc->chselr.chsel17 = 0; // vref
	adc->ccr.tsen = 0;
#elif defined (CONF_L4)
	adc->ccr.chsel17 = 0; // temp
#endif
	adc->ccr.vrefen = 0;

	adc->cr.addis = 1;
	while (adc->cr.aden);
	return t;
}

#endif
