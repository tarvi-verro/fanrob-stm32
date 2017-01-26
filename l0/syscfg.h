#pragma once
#include <stdint.h>
#include <stddef.h>

struct syscfg_cfgr1 {
	uint32_t mem_mode : 2, : 1, ufb : 1, : 4, boot_mode : 2, : 22;
};

struct syscfg_cfgr2 {
	uint32_t fwdis : 1, : 7, i2c_pb6_fmp : 1, i2c_pb7_fmp : 1,
		 i2c_pb8_fmp : 1, i2c_pb9_fmp : 1, i2c1_fmp : 1, i2c2_fmp : 1,
		 i2c3_fmp : 1, : 17;
};

struct syscfg_cfgr3 {
	uint32_t en_vrefint : 1, : 3, sel_vref_out : 2, : 2,
		 enbuf_vrefint_adc : 1, enbuf_sensor_adc : 1, : 2,
		 enbuf_vrefint_comp2 : 1, : 3, : 14, vrefint_rdyf : 1,
		 ref_lock : 1;
};

enum {
	SYSCFG_EXTI_PA,
	SYSCFG_EXTI_PB,
	SYSCFG_EXTI_PC,
	SYSCFG_EXTI_PD,
	SYSCFG_EXTI_PE,
	SYSCFG_EXTI_PH = 5,
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


struct syscfg_reg {
	struct syscfg_cfgr1 cfgr1;		// 0x00
	struct syscfg_cfgr2 cfgr2;		// 0x04
	struct syscfg_exticr1 exticr1;		// 0x08
	struct syscfg_exticr2 exticr2;		// 0x0C
	struct syscfg_exticr3 exticr3;		// 0x10
	struct syscfg_exticr4 exticr4;		// 0x14
	uint32_t comp1_ctrl;			// 0x18
	uint32_t comp2_ctrl;			// 0x1C
	struct syscfg_cfgr3 cfgr3;		// 0x20
};

_Static_assert (offsetof(struct syscfg_reg, cfgr3) == 0x20,
		"SYSCFG registers don't add up.");
