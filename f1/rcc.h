#pragma once
#include <stdint.h>
#include <stddef.h>

struct rcc_apb2enr {
	uint32_t afioen : 1, : 1, iopaen : 1, iopben : 1, iopcen : 1,
		 iopden : 1, iopeen : 1, iopfen : 1, iopgen : 1, adc1en : 1,
		 adc2en : 1, tim1en : 1, spi1en : 1, tim8en : 1, usart1en : 1,
		 adc3en : 1, : 3, tim9en : 1, tim10en : 1, tim11en : 1, : 10;
};

struct rcc_reg {
	uint32_t cr;
	uint32_t cfgr;
	uint32_t cir;
	uint32_t apb2rstr;
	uint32_t apb1rstr;
	uint32_t ahbenr;
	struct rcc_apb2enr apb2enr;
	uint32_t apb1enr;
	uint32_t bdcr;
	uint32_t csr;
};

_Static_assert (offsetof(struct rcc_reg, csr) == 0x24,
		"RCC registers wrong size.");
