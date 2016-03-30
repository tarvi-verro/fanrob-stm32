#pragma once
#include <stdint.h>
#include <stddef.h>

struct adc_isr {
	uint32_t adrdy : 1, eosmp : 1, eoc : 1, eoseq : 1, ovr : 1, : 2,
		 awd : 1, : 24;
};

struct adc_ier {
	uint32_t adrdyie : 1, eosmpie : 1, eocie : 1, eoseqie : 1, ovrie : 1,
		 : 2, awdie : 1, : 24;
};

struct adc_cr {
	uint32_t aden : 1, addis : 1, adstart : 1, : 1, adstp : 1, : 26,
		 adcal : 1;
};

struct adc_cfgr {
	uint32_t dmaen : 1, dmacfg : 1, scandir : 1, res : 2, align : 1,
		 extsel : 3, : 1, exten : 2, ovrmod : 1, cont : 1, wait : 1,
		 autoff : 1, discen : 1, : 5, awdsgl : 1, awden : 1, : 2,
		 awdch : 5, : 1;
	uint32_t : 30, ckmode : 2;
};

_Static_assert (sizeof(struct adc_cfgr) == 0x8,
		"ADC cfgr register should be two long.");

struct adc_smpr {
	uint32_t smp : 3, : 29;
};

struct adc_tr {
	uint32_t lt : 12, : 4, ht : 12, : 4;
};

struct adc_chselr {
	uint32_t chsel0 : 1, chsel1 : 1, chsel2 : 1, chsel3 : 1, chsel4 : 1,
		 chsel5 : 1, chsel6 : 1, chsel7 : 1, chsel8 : 1, chsel9 : 1,
		 chsel10 : 1, chsel11 : 1, chsel12 : 1, chsel13 : 1,
		 chsel14 : 1, chsel15 : 1, chsel16 : 1, chsel17 : 1, : 14;
};

struct adc_dr {
	uint32_t data : 16, : 16;
};

struct adc_ccr {
	uint32_t : 22, vrefen : 1, tsen : 1, : 8;
};

struct adc_reg {
	struct adc_isr isr;
	struct adc_ier ier;
	struct adc_cr cr;
	struct adc_cfgr cfgr;
	struct adc_smpr smpr;
	uint32_t _res0[2];
	struct adc_tr tr;
	uint32_t _res1;
	struct adc_chselr chselr;
	uint32_t _res2[5];
	struct adc_dr dr;
	uint32_t _res3[177];
	struct adc_ccr ccr;
};

_Static_assert (offsetof(struct adc_reg, ccr) == 0x308,
		"ADC registers do not add up.");
