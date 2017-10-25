#pragma once
#include <stdint.h>
#include <stddef.h>

struct adc_isr {
	uint32_t adrdy : 1, eosmp : 1, eoc : 1, eos : 1, ovr : 1, jeoc : 1,
		 jeos : 1, awd1 : 1, awd2 : 1, awd3 : 1, jqovf : 1, : 21;
};

struct adc_cr {
	uint32_t aden : 1, addis : 1, adstart : 1, jadstart : 1, adstp : 1,
		 jadstp : 1, : 10, : 12, advregen : 1, deeppwd : 1,
		 adcaldif : 1, adcal : 1;
};

struct adc_cfgr {
	uint32_t dmaen : 1, dmacfg : 1, : 1, : 2, align : 1, extsel : 4,
		 exten : 2, ovrmod : 1, cont : 1, autdly : 1, : 1, discen : 1,
		 discnum : 3, jdisken : 1, jqm : 1, awd1sgl : 1, awd1en : 1,
		 jawd1en : 1, jauto : 1, awd1ch : 5, jqdis : 1;
};

struct adc_smpr1 {
	uint32_t smp0 : 3, smp1 : 3, smp2 : 3, smp3 : 3, smp4 : 3, smp5 : 3,
		 smp6 : 3, smp7 : 3, smp8 : 3, smp9 : 3, : 2;
};

struct adc_smpr2 {
	uint32_t smp10 : 3, smp11 : 3, smp12 : 3, smp13 : 3, smp14 : 3,
		 smp15 : 3, smp16 : 3, smp17 : 3, smp18 : 3, : 5;
};

enum {
	ADC_LENGTH_1,
	ADC_LENGTH_2,
	ADC_LENGTH_3,
	ADC_LENGTH_4,
	ADC_LENGTH_5,
	ADC_LENGTH_6,
	ADC_LENGTH_7,
	ADC_LENGTH_8,
	ADC_LENGTH_9,
	ADC_LENGTH_10,
	ADC_LENGTH_11,
	ADC_LENGTH_12,
	ADC_LENGTH_13,
	ADC_LENGTH_14,
	ADC_LENGTH_15,
	ADC_LENGTH_16,
};

struct adc_sqr1 {
	uint32_t l : 4, : 2, sq1 : 5, : 1, sq2 : 5, : 1, sq3 : 5, : 1, sq4 : 5,
		 : 3;
};

struct adc_sqr2 {
	uint32_t sq5 : 5, : 1, sq6 : 5, : 1, sq7 : 5, : 1, sq8 : 5, : 1,
		 sq9 : 5, : 3;
};

struct adc_sqr3 {
	uint32_t sq10 : 5, : 1, sq11 : 5, : 1, sq12 : 5, : 1, sq13 : 5, : 1,
		 sq14 : 5, : 3;
};

struct adc_sqr4 {
	uint32_t sq15 : 5, : 1, sq16 : 5, : 21;
};

union adc_dr {
	struct {
		uint32_t rdata : 16, : 16;
	};
	struct { // Compatability
		uint32_t data : 16, : 16;
	};
};

struct adc_ccr {
	uint32_t dual : 5, : 3, delay : 4, : 1, dmacfg : 1, mdma : 2,
		 ckmode : 2, presc : 4, vrefen : 1, chsel17 : 1, chsel18 : 1,
		 : 7;
};


struct adc_reg {
	union { struct adc_isr isr; uint32_t isr32; };		// 0x00
	uint32_t ier32;						// 0x04
	union { struct adc_cr cr; uint32_t cr32; };		// 0x08
	union { struct adc_cfgr cfgr; uint32_t cfgr32; };	// 0x0C
	uint32_t cfgr2_32;					// 0x10
	union { struct adc_smpr1 smpr1; uint32_t smpr1_32; };	// 0x14
	union { struct adc_smpr2 smpr2; uint32_t smpr2_32; };	// 0x18
	uint32_t _res0[1];					// 0x1C
	uint32_t tr1_32;					// 0x20
	uint32_t tr2_32;					// 0x24
	uint32_t tr3_32;					// 0x28
	uint32_t _res1[1];					// 0x2C
	struct adc_sqr1 sqr1;					// 0x30
	struct adc_sqr2 sqr2;					// 0x34
	struct adc_sqr3 sqr3;					// 0x38
	struct adc_sqr4 sqr4;					// 0x3C
	union { union adc_dr dr; uint32_t dr32; };		// 0x40
	uint32_t _res2[2];					// 0x44
	uint32_t jsqr32;					// 0x4C
	uint32_t _res3[4];					// 0x50
	uint32_t ofr1_32;					// 0x60
	uint32_t ofr2_32;					// 0x64
	uint32_t ofr3_32;					// 0x68
	uint32_t ofr4_32;					// 0x6C
	uint32_t _res4[4];					// 0x70
	uint32_t jdr1_32;					// 0x80
	uint32_t jdr2_32;					// 0x84
	uint32_t jdr3_32;					// 0x88
	uint32_t jdr4_32;					// 0x8C
	uint32_t _res5[4];					// 0x90
	uint32_t awd2cr32;					// 0xA0
	uint32_t awd3cr32;					// 0xA4
	uint32_t _res6[2];					// 0xA8
	uint32_t difsel32;					// 0xB0
	uint32_t calfact32;					// 0xB4

	uint32_t _res7[0x92];					// 0xB8

								//-0x300
	uint32_t csr32;						// 0x00
	uint32_t _res8[1];					// 0x04
	union { struct adc_ccr ccr; uint32_t ccr32; };		// 0x08
	uint32_t cdr32;						// 0x0C

};

_Static_assert (offsetof(struct adc_reg, dr) == 0x40,
		"ADC registers mismatch at rdata register.");

_Static_assert (offsetof(struct adc_reg, calfact32) == 0xB4,
	       "ADC registers mismatch.");

_Static_assert (offsetof(struct adc_reg, csr32) == 0x300,
	       "ADC registers mismatch.");

