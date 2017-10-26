#pragma once

extern void setup_adc();

/**
 * get_temp() - get the cpu temperature
 *
 * Return: t°C
 */
extern int get_temp();

/**
 * get_vdda() - get the operating voltage
 *
 * Return: mV
 */
extern int get_vdda();

#ifdef CONF_L4
extern int get_hvpwr_vdd();
#endif


#ifdef CFG_ADC
struct adc_configuration {
	enum pin hvpwr;
	int hvpwr_adc_in;
	int r1, r2;
	int v_offset; // in mV
};
static const struct adc_configuration cfg_adc;
#endif
