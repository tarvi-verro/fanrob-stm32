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
	uint32_t dma_en : 1, : 1, sram_en : 1, : 1, flitf_en : 1, : 1,
		 crc_en : 1, : 10, iopa_en : 1, iopb_en : 1, iopc_en : 1,
		 iopd_en : 1, : 1, iopf_en : 1, : 9;

};

struct rcc_apb2enr {
	uint32_t syscfg_en : 1, : 4, usart6_en : 1, : 3, adc_en : 1, : 1,
		 tim1_en : 1, spi1_en : 1, : 1, usart1_en : 1, : 1,
		 tim15_en : 1, tim16_en : 1, tim17_en : 1, : 3, dbgmcu_en : 1,
		 : 9;
};

struct rcc_apb1enr {
	uint32_t : 1, tim3_en : 1, : 2, tim6_en : 1, tim7_en : 1, : 2,
		 wwdg_en : 1, : 2, spi2_en : 1, : 2, usart2_en : 1,
		 usart3_en : 1, usart4_en : 1, usart5_en : 1, i2c1_en : 1,
		 i2c2_en : 1, usb_en : 1, : 4, pwr_en : 1, : 3;
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
	uint32_t bdcr;
	uint32_t csr;
	uint32_t ahbrstr;
	uint32_t cfgr2;
	uint32_t cfgr3;
	uint32_t cr2;
};

_Static_assert (offsetof(struct rcc_reg, cr2) == 0x34,
		"RCC registers have gone bad.");
