#pragma once
#include <stdint.h>
#include <stddef.h>
#include <assert-c.h>

struct adc_sr {
	uint32_t awd : 1, eoc : 1, jeoc : 1, jstrt : 1, strt : 1, : 27;
};

struct adc_cr2 {
	uint32_t adon : 1, cont : 1, cal : 1, rstcal : 1, : 4, dma : 1,  : 2,
		 align : 1, jextsel : 3, jexttrig : 1, : 1, extsel : 3,
		 exttrig : 1, jswstart : 1, swstart : 1, tsvrefe : 1, : 8;
};

enum adc_smp {
	ADC_SMP_1_7,
	ADC_SMP_7_5,
	ADC_SMP_13_5,
	ADC_SMP_28_5,
	ADC_SMP_41_5,
	ADC_SMP_55_5,
	ADC_SMP_71_5,
	ADC_SMP_239_5,
};
static inline enum adc_smp get_adc_smp(unsigned sampling_time_ns, unsigned adcclk_cycle_ns) {
	// round up
	unsigned dcycles = (10 * sampling_time_ns + adcclk_cycle_ns - 1) / adcclk_cycle_ns;
	if (dcycles <= 17)
		return ADC_SMP_1_7;
	else if (dcycles <= 75)
		return ADC_SMP_7_5;
	else if (dcycles <= 135)
		return ADC_SMP_13_5;
	else if (dcycles <= 285)
		return ADC_SMP_28_5;
	else if (dcycles <= 415)
		return ADC_SMP_41_5;
	else if (dcycles <= 555)
		return ADC_SMP_55_5;
	else if (dcycles <= 715)
		return ADC_SMP_71_5;
	else if (dcycles <= 2395)
		return ADC_SMP_239_5;
	else assert(0);
	return ADC_SMP_239_5;
}

_Static_assert (ADC_SMP_239_5 == 7, "adc_smp mismatch");


struct adc_smpr1 {
	enum adc_smp smp10 : 3, smp11 : 3, smp12 : 3, smp13 : 3, smp14 : 3,
		     smp15 : 3, smp16 : 3, smp17 : 3;
	uint32_t : 8;
} __attribute__ ((__packed__));
_Static_assert (sizeof(struct adc_smpr1) == 4, "adc_smpr1 bitfield packing error");

struct adc_smpr2 {
	enum adc_smp smp0 : 3, smp1 : 3, smp2 : 3, smp3 : 3, smp4 : 3,
		     smp5 : 3, smp6 : 3, smp7 : 3, smp8 : 3, smp9 : 3;
	uint32_t : 2;
} __attribute__ ((__packed__));
_Static_assert (sizeof(struct adc_smpr2) == 4, "adc_smpr2 bitfield packing error");

struct adc_sqr1 {
	uint32_t sq13 : 5, sq14 : 5, sq15 : 5, sq16 : 5, l : 4, : 8;
};

struct adc_sqr2 {
	uint32_t sq7 : 5, sq8 : 5, sq9 : 5, sq10 : 5, sq11 : 5, sq12 : 5, : 2;
};

struct adc_sqr3 {
	uint32_t sq1 : 5, sq2 : 5, sq3 : 5, sq4 : 5, sq5 : 5, sq6: 5, : 2;
};

struct adc_dr {
	uint32_t data : 16, adc2data : 16;
};

struct adc_reg {
	struct adc_sr sr;					// 0x00
	uint32_t cr1_32;					// 0x04
	struct adc_cr2 cr2;					// 0x08
	struct adc_smpr1 smpr1;					// 0x0C
	struct adc_smpr2 smpr2;					// 0x10
	uint32_t jofr1_32;					// 0x14
	uint32_t jofr2_32;					// 0x18
	uint32_t jofr3_32;					// 0x1C
	uint32_t jofr4_32;					// 0x20
	uint32_t htr_32;					// 0x24
	uint32_t ltr_32;					// 0x28
	struct adc_sqr1 sqr1;					// 0x2C
	struct adc_sqr2 sqr2;					// 0x30
	struct adc_sqr3 sqr3;					// 0x34
	uint32_t jsqr_32;					// 0x38
	uint32_t jdr1_32;					// 0x3C
	uint32_t jdr2_32;					// 0x40
	uint32_t jdr3_32;					// 0x44
	uint32_t jdr4_32;					// 0x48
	struct adc_dr dr;					// 0x4C
};

_Static_assert (offsetof(struct adc_reg, dr) == 0x4C,
	       "ADC registers mismatch.");

