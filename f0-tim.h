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
	uint32_t ccpc : 1, : 1, ccus : 1, ccds : 1, mms : 3, ti1s : 1,
		 ois1 : 1, ois1n : 1, ois2 : 1, ois2n : 1, ois3 : 1, ois3n : 1,
		 ois4 : 1, : 1, : 16; /* Tim3 doesnt have all of these */
};

struct tim_smcr {
	uint32_t sms : 3, occs : 1, ts : 3, msm : 1, etf : 4, etps : 2,
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
	uint32_t ug : 1, cc1g : 1, cc2g : 1, cc3g : 1, cc4g : 1, comg : 1,
		 tg : 1, bg : 1, : 24;
};

struct tim_ccer {
	uint32_t cc1e : 1, cc1p : 1, cc1ne : 1, cc1np : 1,
		 cc2e : 1, cc2p : 1, cc2ne : 1, cc2np : 1,
		 cc3e : 1, cc3p : 1, cc3ne : 1, cc3np : 1,
		 cc4e : 1, cc4p : 1, : 2, : 16;
};

struct tim_bdtr {
	uint32_t dtg : 8, lock : 2, ossi : 1, ossr : 1, bke : 1, bkp : 1,
		 aoe : 1, moe : 1, : 16;
};

struct tim_reg {
	struct tim_cr1 cr1;
	struct tim_cr2 cr2;
	struct tim_smcr smcr;
	uint32_t dier; /* dma stuff */
	uint32_t sr;
	struct tim_egr egr;
	struct tim_ccmr ccmr;
	struct tim_ccer ccer;
	uint32_t cnt;
	uint32_t psc;
	uint32_t arr;
	uint32_t rcr;
	uint32_t ccr1;
	uint32_t ccr2;
	uint32_t ccr3;
	uint32_t ccr4;
	struct tim_bdtr bdtr;
	uint32_t dcr;
	uint32_t dmar;
};

_Static_assert (offsetof(struct tim_reg, dmar) == 0x4c,
		"TIM registers have failed.");
