#pragma once
#define _RCC_ABS_H
#include "rcc.h"
#include "regs.h"

static inline unsigned rcc_get_sysclk()
{
	unsigned msilup[] = {
		[MSIRANGE0_100_kHz] = 100000,
		[MSIRANGE1_200_kHz] = 200000,
		[MSIRANGE2_400_kHz] = 400000,
		[MSIRANGE3_800_kHz] = 800000,
		[MSIRANGE4_1_MHz] = 1000000,
		[MSIRANGE5_2_MHz] = 2000000,
		[MSIRANGE6_4_MHz] = 4000000,
		[MSIRANGE7_8_MHz] = 8000000,
		[MSIRANGE8_16_MHz] = 16000000,
		[MSIRANGE9_24_MHz] = 24000000,
		[MSIRANGE10_32_MHz] = 32000000,
		[MSIRANGE11_48_MHz] = 48000000,
	};


	if (rcc->cfgr.sw == SW_MSI) {
		switch (rcc->cr.msirgsel) {
		case MSIRGSEL_MSISRANGE:
			return msilup[rcc->csr.msisrange];
		case MSIRGSEL_MSIRANGE:
			return msilup[rcc->cr.msirange];
		};
	}

	assert(0);
	return 0;
}
