#pragma once
#include <stdint.h>
#include <stddef.h>


enum {
	TIM_DIR_UPCNT,
	TIM_DIR_DOWNCNT,
};
struct tim_cr1 {
	uint32_t cen : 1, udis : 1, urs : 1, opm : 1, dir : 1, cms : 2,
		 arpe : 1, ckd : 2, : 6, : 16;
};

struct tim_cr2 {
	uint32_t : 3, ccds : 1, mms : 3, ti1s : 1,
		 : 8, : 16; /* Tim3 doesnt have all of these */
};

struct tim_smcr {
	uint32_t sms : 3, : 1, ts : 3, msm : 1, etf : 4, etps : 2,
		 ece : 1, etp : 1, : 16;
};

enum {
	TIM_CCS_OUT,
	TIM_CCS_IN_TI1,
	TIM_CCS_IN_TI2,
	TIM_CCS_IN_TRC,
};
struct tim_ccmr_out_ch {
	uint8_t ccs : 2, ocfe : 1, ocpe : 1, ocm : 3, occe : 1;
};

struct tim_ccmr_in_ch {
	uint8_t ccs : 2, icpsc : 2, icf : 4;
};

union tim_ccmr_ch {
	struct tim_ccmr_out_ch out;
	struct tim_ccmr_in_ch in;
};

struct tim_ccmr {
	union tim_ccmr_ch ch1;
	union tim_ccmr_ch ch2;
	uint16_t _pad0;
	union tim_ccmr_ch ch3;
	union tim_ccmr_ch ch4;
	uint16_t _pad1;
};

struct tim_egr {
	uint32_t ug : 1, cc1g : 1, cc2g : 1, cc3g : 1, cc4g : 1, : 1,
		 tg : 1, : 9, : 16;
};

struct tim_ccer {
	uint32_t cc1e : 1, cc1p : 1, : 1, cc1np : 1,
		 cc2e : 1, cc2p : 1, : 1, cc2np : 1,
		 cc3e : 1, cc3p : 1, : 1, cc3np : 1,
		 cc4e : 1, cc4p : 1, : 1, cc4np : 1, : 16;
};

struct tim_reg {
	struct tim_cr1 cr1;		// 0x00
	struct tim_cr2 cr2;		// 0x04
	struct tim_smcr smcr;		// 0x08
	uint32_t dier; /* dma stuff */	// 0x0C
	uint32_t sr;			// 0x10
	struct tim_egr egr;		// 0x14
	struct tim_ccmr ccmr;		// 0x18
	struct tim_ccer ccer;		// 0x20
	uint32_t cnt;			// 0x24
	uint32_t psc;			// 0x28
	uint32_t arr;			// 0x2C
	uint32_t _padding0[1];		// 0x30
	uint32_t ccr1;			// 0x34
	uint32_t ccr2;			// 0x38
	uint32_t ccr3;			// 0x3C
	uint32_t ccr4;			// 0x40
	uint32_t _padding1[1];		// 0x44
	uint32_t dcr;			// 0x48
	uint32_t dmar;			// 0x4C
	/* or has different bitfields for tim2 and 3 */
	uint32_t or;			// 0x50
};

_Static_assert (offsetof(struct tim_reg, or) == 0x50,
		"TIM registers have failed.");
