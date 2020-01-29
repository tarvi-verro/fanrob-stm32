#pragma once
#include <stdint.h>
#include <stddef.h>

struct adc_reg {
	uint32_t sr_32;						// 0x00
	uint32_t cr1_32;					// 0x04
	uint32_t cr2_32;					// 0x08
	uint32_t smpr1_32;					// 0x0C
	uint32_t smpr2_32;					// 0x10
	uint32_t jofr1_32;					// 0x14
	uint32_t jofr2_32;					// 0x18
	uint32_t jofr3_32;					// 0x1C
	uint32_t jofr4_32;					// 0x20
	uint32_t htr_32;					// 0x24
	uint32_t ltr_32;					// 0x28
	uint32_t sqr1_32;					// 0x2C
	uint32_t sqr2_32;					// 0x30
	uint32_t sqr3_32;					// 0x34
	uint32_t jsqr_32;					// 0x38
	uint32_t jdr1_32;					// 0x3C
	uint32_t jdr2_32;					// 0x40
	uint32_t jdr3_32;					// 0x44
	uint32_t jdr4_32;					// 0x48
	uint32_t dr_32;						// 0x4C
};

_Static_assert (offsetof(struct adc_reg, dr_32) == 0x4C,
	       "ADC registers mismatch.");

