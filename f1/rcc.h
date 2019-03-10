#pragma once
#include <stdint.h>
#include <stddef.h>

struct rcc_cr {
	uint32_t hsion : 1, hsirdy : 1, : 1, hsitrim : 5, hsical : 8,
		 hseon : 1, hserdy : 1, hsebyp : 1, csson : 1, : 4, pllon : 1,
		 pllrdy : 1, : 6;
};

enum rcc_cfgr_sw {
	RCC_SW_HSI,
	RCC_SW_HSE,
	RCC_SW_PLL,
};
enum rcc_cfgr_pllmul {
	RCC_PLLMUL_X2,
	RCC_PLLMUL_X3,
	RCC_PLLMUL_X4,
	RCC_PLLMUL_X5,
	RCC_PLLMUL_X6,
	RCC_PLLMUL_X7,
	RCC_PLLMUL_X8,
	RCC_PLLMUL_X9,
	RCC_PLLMUL_X10,
	RCC_PLLMUL_X11,
	RCC_PLLMUL_X12,
	RCC_PLLMUL_X13,
	RCC_PLLMUL_X14,
	RCC_PLLMUL_X15,
	RCC_PLLMUL_X16,
	_RCC_PLLMUL_X16,
};
enum rcc_cfgr_pllsrc {
	RCC_PLLSRC_HSI_DIV2,
	RCC_PLLSRC_HSE,
};
enum rcc_cfgr_hpre {
	RCC_HPRE_SYSCLK_NOT_DIV = 0,
	RCC_HPRE_SYSCLK_DIV_2 = 0x8,
	RCC_HPRE_SYSCLK_DIV_4,
	RCC_HPRE_SYSCLK_DIV_8,
	RCC_HPRE_SYSCLK_DIV_16,
	RCC_HPRE_SYSCLK_DIV_64,
	RCC_HPRE_SYSCLK_DIV_128,
	RCC_HPRE_SYSCLK_DIV_256,
	RCC_HPRE_SYSCLK_DIV_512,
};
struct rcc_cfgr {
	enum rcc_cfgr_sw sw : 2, sws : 2;
	enum rcc_cfgr_hpre hpre : 4;
	uint32_t ppre1 : 3, ppre2 : 3, adcpre : 2;
	enum rcc_cfgr_pllsrc pllsrc : 1;
	uint32_t pllxtpre : 1;
	enum rcc_cfgr_pllmul pllmul : 4;
	uint32_t usbpre : 1, : 1, mco : 3, : 5;
};

struct rcc_apb2enr {
	uint32_t afioen : 1, : 1, iopaen : 1, iopben : 1, iopcen : 1,
		 iopden : 1, iopeen : 1, iopfen : 1, iopgen : 1, adc1en : 1,
		 adc2en : 1, tim1en : 1, spi1en : 1, tim8en : 1, usart1en : 1,
		 adc3en : 1, : 3, tim9en : 1, tim10en : 1, tim11en : 1, : 10;
};

struct rcc_apb1enr {
	uint32_t tim2en : 1, tim3en : 1, tim4en : 1, tim5en : 1, tim6en : 1,
		 tim7en : 1, tim12en :1, tim13en : 1, tim14en : 1, : 2,
		 wwdgen : 1, : 2, spi2en : 1, spi3en : 1, : 1, usart2en : 1,
		 usart3en : 1, uart4en : 1, uart5en : 1, i2c1en : 1,
		 i2c2en : 1, usben : 1, : 1, canen : 1, : 1, bkpen : 1,
		 pwren : 1, dacen : 1, : 2;
};

struct rcc_reg {
	struct rcc_cr cr;
	struct rcc_cfgr cfgr;
	uint32_t cir;
	uint32_t apb2rstr;
	uint32_t apb1rstr;
	uint32_t ahbenr;
	struct rcc_apb2enr apb2enr;
	struct rcc_apb1enr apb1enr;
	uint32_t bdcr;
	uint32_t csr;
};

_Static_assert (offsetof(struct rcc_reg, csr) == 0x24,
		"RCC registers wrong size.");
