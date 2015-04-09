#include <stdint.h>
#include <stddef.h>

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

struct rcc_reg {
	uint32_t cr;
	uint32_t cfgr;
	uint32_t cir;
	uint32_t apb2rstr;
	uint32_t apb1rstr;
	struct rcc_ahbenr ahbenr;
	struct rcc_apb2enr apb2enr;
	uint32_t apb1enr;
	uint32_t bdcr;
	uint32_t csr;
	uint32_t ahbrstr;
	uint32_t cfgr2;
	uint32_t cfgr3;
	uint32_t cr2;
};

_Static_assert (offsetof(struct rcc_reg, cr2) == 0x34,
		"RCC registers have gone bad.");
