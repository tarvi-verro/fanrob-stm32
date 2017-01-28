#pragma once
#include <stdint.h>
#include <stddef.h>

enum {
	RCC_P_PRE_NONE,
	RCC_P_PRE_2 = 4,
	RCC_P_PRE_4,
	RCC_P_PRE_8,
	RCC_P_PRE_16,
};
_Static_assert (RCC_P_PRE_16 == 7, "RCC_P_PRE broken.");
struct rcc_cfgr {
	uint32_t sw : 2, sws : 2, h_pre : 4, p_pre : 3, : 3, adc_pre : 1, : 1,
		 pll_src : 1, pllxt_pre : 1, pll_mul : 4, : 2, mco : 4,
		 mco_pre : 3, pll_nodiv : 1;
};

struct rcc_ahbenr {
	uint32_t dmaen : 1, : 1, sramen : 1, : 1, flitfen : 1, : 1,
		 crcen : 1, : 10, iopaen : 1, iopben : 1, iopcen : 1,
		 iopden : 1, : 1, iopfen : 1, : 9;

};

struct rcc_apb2enr {
	uint32_t syscfgen : 1, : 4, usart6en : 1, : 3, adcen : 1, : 1,
		 tim1en : 1, spi1en : 1, : 1, usart1en : 1, : 1,
		 tim15en : 1, tim16en : 1, tim17en : 1, : 3, dbgmcuen : 1,
		 : 9;
};

struct rcc_apb1enr {
	uint32_t : 1, tim3en : 1, : 2, tim6en : 1, tim7en : 1, : 2,
		 tim14en : 1, : 2, wwdgen : 1, : 2, spi2en : 1, : 2,
		 usart2en : 1, usart3en : 1, usart4en : 1, usart5en : 1,
		 i2c1en : 1, i2c2en : 1, usben : 1, : 4, pwren : 1, : 3;
};


enum {
	RCC_RTCSEL_NONE,
	RCC_RTCSEL_LSE,
	RCC_RTCSEL_LSI,
	RCC_RTCSEL_HSE,
};
struct rcc_bdcr {
	uint32_t lseon : 1, lserdy : 1, lsebyp : 1, lsedrv : 2, : 3,
		 rtcsel : 2, : 5, rtcen : 1, bdrst : 1, : 15;
};

struct rcc_csr {
	uint32_t lsion : 1, lsirdy : 1, : 21, v18pwr_rstf : 1, rmvf : 1,
		 obl_rstf : 1, pin_rstf : 1, por_rstf : 1, sft_rstf : 1,
		 iwdg_rstf : 1, wwdg_rstf : 1, lpwr_rstf : 1;
};

struct rcc_reg {
	uint32_t cr;
	struct rcc_cfgr cfgr;
	uint32_t cir;
	uint32_t apb2rstr;
	uint32_t apb1rstr;
	struct rcc_ahbenr ahbenr;
	struct rcc_apb2enr apb2enr;
	struct rcc_apb1enr apb1enr;
	struct rcc_bdcr bdcr;
	struct rcc_csr csr;
	uint32_t ahbrstr;
	uint32_t cfgr2;
	uint32_t cfgr3;
	uint32_t cr2;
};

_Static_assert (offsetof(struct rcc_reg, cr2) == 0x34,
		"RCC registers have gone bad.");
