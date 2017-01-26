#pragma once
#include <stdint.h>
#include <stddef.h>

enum {
	RCC_PPRE_NONE,
	RCC_PPRE_2 = 4,
	RCC_PPRE_4,
	RCC_PPRE_8,
	RCC_PPRE_16,
};
_Static_assert (RCC_PPRE_16 == 7, "RCC_PPRE broken.");

struct rcc_cfgr {
	uint32_t sw : 2, sws : 2, hpre : 4, ppre1 : 3, ppre2 : 3, : 1,
		 stopwuck : 1, pllsrc : 1, : 1, pllmul : 4, plldiv : 2,
		 mcosel : 4, mcopre : 3, : 1;
};

struct rcc_iopenr {
	uint32_t iopaen : 1, iopben : 1, iopcen : 1, iopden : 1, iopeen : 1,
		 : 2, iophen : 1, : 24;
};

struct rcc_ahbenr {
	uint32_t dma1en : 1, : 7, mifen : 1, : 3, crcen : 1,
		 : 3, : 1, : 3, : 1, : 3, crypen : 1, : 7;

};

struct rcc_apb2enr {
	uint32_t syscfgen : 1, : 1, tim21en : 1, : 2, tim22en : 1, : 1,
		 fwen : 1, : 1, adcen : 1, : 2, spi1en : 1, : 1, usart1en : 1,
		 : 7, dbgen : 1, : 9;
};

struct rcc_apb1enr {
	uint32_t tim2en : 1, tim3en : 1, : 2, tim6en : 1, tim7en : 1, : 5,
		 wwdgen : 1, : 2, spi2en : 1, : 2, usart2en : 1, lpuart1en : 1,
		 usart4en : 1, usart5en : 1, i2c1en : 1, i2c2en : 1, : 5,
		 pwren : 1, : 1, i2c3en : 1, lptim1en : 1;
};


enum {
	RCC_RTCSEL_NONE,
	RCC_RTCSEL_LSE,
	RCC_RTCSEL_LSI,
	RCC_RTCSEL_HSE,
};
struct rcc_csr {
	uint32_t lsion : 1, lsirdy : 1, : 6, lseon : 1, lserdy : 1, lsebyp : 1,
		 lsedrv : 2, csslseon : 1, csslsed : 1, : 1, rtcsel : 2,
		 rtcen : 1, rtcrst : 1, : 3, rmvf : 1, fwrstf : 1, oblrstf : 1,
		 pinrstf : 1, porrstf : 1, sftrstf : 1, iwdgrstf : 1,
		 wwdgrstf : 1, lpwrrstf : 1;
};

enum {
	RCC_CLKSRC_PCLK,
	RCC_CLKSRC_SYSCLK,
	RCC_CLKSRC_HSI16,
	RCC_CLKSRC_LSE,
};

struct rcc_ccipr {
	uint32_t usart1sel : 2, usart2sel : 2, : 6, lpuart1sel : 2,
		 i2c1sel : 2, : 2, i2c3sel : 2, lptim1sel : 2, : 12;
};

struct rcc_reg {
	uint32_t cr;			// 0x00
	uint32_t icscr;			// 0x04
	uint32_t _padding00[1];		// 0x08
	struct rcc_cfgr cfgr;		// 0x0C
	uint32_t cier;			// 0x10
	uint32_t cifr;			// 0x14
	uint32_t cicr;			// 0x18
	uint32_t ioprstr;		// 0x1C
	uint32_t ahbrstr;		// 0x20
	uint32_t apb2rstr;		// 0x24
	uint32_t apb1rstr;		// 0x28
	struct rcc_iopenr iopenr;	// 0x2C
	struct rcc_ahbenr ahbenr;	// 0x30
	struct rcc_apb2enr apb2enr;	// 0x34
	struct rcc_apb1enr apb1enr;	// 0x38
	uint32_t iopsmen;		// 0x3C
	uint32_t ahbsmenr;		// 0x40
	uint32_t apb2smenr;		// 0x44
	uint32_t apb1smenr;		// 0x48
	struct rcc_ccipr ccipr;		// 0x4C
	struct rcc_csr csr;		// 0x50
};

_Static_assert (offsetof(struct rcc_reg, csr) == 0x50,
		"RCC registers have gone bad.");

