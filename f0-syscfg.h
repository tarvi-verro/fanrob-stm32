#pragma once
#include <stdint.h>
#include <stddef.h>

struct syscfg_cfgr1 {
	uint32_t mem_mode : 2, : 2, pa11_pa12_rmp : 1, : 3, adc_dma_rmp : 1,
		 usart1_tx_dma_rmp : 1, usart1_rx_dma_rmp : 1,
		 tim16_dma_rmp : 1, tim17_dma_rmp : 1, : 3, i2c_pb6_fmp : 1,
		 i2c_pb7_fmp : 1, i2c_pb8_fmp : 1, i2c_pb9_fmp : 1,
		 i2c1_fmp : 1, : 1, i2c_pa9_fmp :1 , i2c_pa10_fmp : 1, : 2,
		 usart3_dma_rmp : 1, : 5;
};

enum {
	SYSCFG_EXTI_PA,
	SYSCFG_EXTI_PB,
	SYSCFG_EXTI_PC,
	SYSCFG_EXTI_PD, /* 4 reserved */
	SYSCFG_EXTI_PF = 5,
};

struct syscfg_exticr1 {
	uint32_t exti0 : 4, exti1 : 4, exti2 : 4, exti3 : 4, : 16;
};

struct syscfg_exticr2 {
	uint32_t exti4 : 4, exti5 : 4, exti6 : 4, exti7 : 4, : 16;
};

struct syscfg_exticr3 {
	uint32_t exti8 : 4, exti9 : 4, exti10 : 4, exti11 : 4, : 16;
};

struct syscfg_exticr4 {
	uint32_t exti12 : 4, exti13 : 4, exti14 : 4, exti15 : 4, : 16;
};

struct syscfg_cfgr2 {
	uint32_t lockup_lock : 1, sram_parity_lock : 1, : 6, sram_pef : 1,
		 : 23;
};

struct syscfg_reg {
	struct syscfg_cfgr1 cfgr1;
	uint32_t _padding;
	struct syscfg_exticr1 exticr1;
	struct syscfg_exticr2 exticr2;
	struct syscfg_exticr3 exticr3;
	struct syscfg_exticr4 exticr4;
	struct syscfg_cfgr2 cfgr2;
};

_Static_assert (offsetof(struct syscfg_reg, cfgr2) == 0x18,
		"SYSCFG registers don't add up.");
