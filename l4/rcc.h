#pragma once
#include <stdint.h>
#include <stddef.h>

enum rcc_msirange {
	MSIRANGE0_100_kHz,
	MSIRANGE1_200_kHz,
	MSIRANGE2_400_kHz,
	MSIRANGE3_800_kHz,
	MSIRANGE4_1_MHz,
	MSIRANGE5_2_MHz,
	MSIRANGE6_4_MHz,
	MSIRANGE7_8_MHz,
	MSIRANGE8_16_MHz,
	MSIRANGE9_24_MHz,
	MSIRANGE10_32_MHz,
	MSIRANGE11_48_MHz,
};

struct rcc_cr {
	uint32_t msion : 1, msirdy : 1, msipllen : 1, msirgsel : 1;
	enum rcc_msirange msirange : 4;
	uint32_t hsion : 1, hsikeron : 1, hsirdy : 1, hsiasfs : 1, : 4,
		 hseon : 1, hserdy : 1, hsebyp : 1, csson : 1, : 4, pllon : 1,
		 pllrdy : 1,  pllsai1on : 1, pllsai1rdy : 1, : 4;
};

enum {
	RCC_PPRE_NONE,
	RCC_PPRE_2 = 4,
	RCC_PPRE_4,
	RCC_PPRE_8,
	RCC_PPRE_16,
};
_Static_assert (RCC_PPRE_16 == 7, "RCC_PPRE broken.");

enum rcc_sw {
	SW_MSI,
	SW_HSI16,
	SW_HSE,
	SW_PLL,
};

struct rcc_cfgr {
	enum rcc_sw sw : 2;
	uint32_t sws : 2, hpre : 4, ppre1 : 3, ppre2 : 3, : 1,
		 stopwuck : 1, : 8, mcosel : 4, mcopre : 3, : 1;
};

struct rcc_ahb1enr {
	uint32_t dma1en : 1, dma2en : 1, : 6, flashen : 1, : 3, crcen : 1,
		 : 3, tscen : 1, : 15;

};

struct rcc_ahb2enr {
	uint32_t gpioaen : 1, gpioben : 1, gpiocen : 1, gpioden : 1,
		 gpioeen : 1, : 2, gpiohen : 1, : 5, adcen : 1, : 2,
		 aesen : 1, : 1, rngen : 1, : 13;
};

struct rcc_apb2enr {
	uint32_t syscfgen : 1, : 6, fwen : 1, : 2, sdmmc1en : 1, tim1en : 1,
		 spi1en : 1, : 1, usart1en : 1, : 1, tim15en : 1, tim16en : 1,
		 : 3, sai1en : 1, : 10;
};

struct rcc_apb1enr2 {
	uint32_t lpuart1en : 1, : 1, swpmi1en : 1, : 2, lptim2en : 1, : 26;
};

struct rcc_apb1enr1 {
	uint32_t tim2en : 1, : 3, tim6en : 1, tim7en : 1, : 4, rtcapben : 1,
		 wwdgen : 1, : 2, spi2en : 1, spi3en : 1, : 1, usart2en : 1,
		 usart3en : 1, : 2, i2c1en : 1, i2c2en : 1, i2c3en : 1,
		 crsen : 1, can1en : 1, usbfsen : 1, : 1, pwren : 1,
		 dac1en : 1, opampen : 1, lptim1en : 1;
};


enum {
	RCC_RTCSEL_NONE,
	RCC_RTCSEL_LSE,
	RCC_RTCSEL_LSI,
	RCC_RTCSEL_HSE,
};
struct rcc_bdcr {
	uint32_t lseon : 1, lserdy : 1, lsebyp : 1, lsedrv : 2, lsecsson : 1,
		 lsecssd : 1, : 1, rtcsel : 2, : 5, rtcen : 1, bdrst : 1,
		 : 7, lscoen : 1, lscosel : 1, : 6;
};

struct rcc_csr {
	uint32_t lsion : 1, lsirdy : 1, : 6, msisrange : 4, : 11, rmvf : 1,
		 fwrstf : 1, oblrstf : 1, pinrstf : 1, borrstf : 1,
		 sftrstf : 1, iwwgrstf : 1, wwdgrstf : 1, lpwrstf : 1;
};

enum {
	RCC_CLKSRC_PCLK,
	RCC_CLKSRC_SYSCLK,
	RCC_CLKSRC_HSI16,
	RCC_CLKSRC_LSE,
};

struct rcc_ccipr {
	uint32_t uart1sel : 2, uart2sel : 2, uart3sel : 2, : 4, lpuart1sel : 2,
		 i2c1sel : 2, i2c2sel : 2, i2c3sel : 2, lptim1sel : 2,
		 lptim2sel : 2, sai1sel : 2, : 2, clk48sel : 2, adcsel : 2,
		 swpmi1sel : 1, : 1;
};

struct rcc_reg {
	struct rcc_cr cr;		// 0x00
	uint32_t icscr;			// 0x04
	struct rcc_cfgr cfgr;		// 0x08
	uint32_t pllcfgr;		// 0x0C
	uint32_t pllsai1cfgr;		// 0x10
	uint32_t _padding0[1];		// 0x14
	uint32_t cier;			// 0x18
	uint32_t cifr;			// 0x1C
	uint32_t cicr;			// 0x20
	uint32_t _padding1[1];		// 0x24
	uint32_t ahb1rstr;		// 0x28
	uint32_t ahb2rstr;		// 0x2C
	uint32_t ahb3rstr;		// 0x30
	uint32_t _padding2[1];		// 0x34
	uint32_t apb1rstr1;		// 0x38
	uint32_t apb1rstr2;		// 0x3C
	uint32_t apb2rstr;		// 0x40
	uint32_t _padding3[1];		// 0x44
	struct rcc_ahb1enr ahb1enr;	// 0x48
	struct rcc_ahb2enr ahb2enr;	// 0x4C
	uint32_t ahb3enr;		// 0x50
	uint32_t _padding4[1];		// 0x54
	struct rcc_apb1enr1 apb1enr1;	// 0x58
	struct rcc_apb1enr2 apb1enr2;	// 0x5C
	struct rcc_apb2enr apb2enr;	// 0x60
	uint32_t _padding5[1];		// 0x64
	uint32_t ahb1smenr;		// 0x68
	uint32_t ahb2smenr;		// 0x6C
	uint32_t ahb3smenr;		// 0x70
	uint32_t _padding6[1];		// 0x74
	uint32_t apb1smenr1;		// 0x78
	uint32_t apb1smenr2;		// 0x7C
	uint32_t apb2smenr;		// 0x80
	uint32_t _padding7[1];		// 0x84
	struct rcc_ccipr ccipr;		// 0x88
	uint32_t _padding8[1];		// 0x8C
	struct rcc_bdcr bdcr;		// 0x90
	uint32_t csr;			// 0x94
	uint32_t crrcr;			// 0x98
};

_Static_assert (offsetof(struct rcc_reg, crrcr) == 0x98,
		"RCC registers have gone bad.");

