#pragma once
#define _RCC_ABS_H
#include "rcc.h"
#include "regs.h"

static inline unsigned rcc_get_sysclk()
{
	unsigned msilup[] = {
		[MSIRANGE0_65_kHz] = 65536,
		[MSIRANGE1_131_kHz] = 131072,
		[MSIRANGE2_262_kHz] = 262144,
		[MSIRANGE3_524_kHz] = 524288,
		[MSIRANGE4_1_MHz] = 1048000,
		[MSIRANGE5_2_MHz] = 2097000,
		[MSIRANGE6_4_MHz] = 4194000,
	};

	if (rcc->cfgr.sw == SW_MSI_OSC)
		return msilup[rcc->icscr.msirange];
	assert(0);
	return 0;
}
