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
	uint32_t sqr1_32;					// 0x30
	uint32_t sqr2_32;					// 0x34
	uint32_t sqr3_32;					// 0x38
	uint32_t sqr4_32;					// 0x3C
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

_Static_assert (offsetof(struct adc_reg, calfact32) == 0xB4,
	       "ADC registers mismatch.");

_Static_assert (offsetof(struct adc_reg, csr32) == 0x300,
	       "ADC registers mismatch.");

