#pragma once
#include <stdint.h>
#include <stddef.h>


struct syscfg_reg {
	uint32_t memrmp;		// 0x00
	uint32_t cfgr1;			// 0x04
	uint32_t exticr1;		// 0x08
	uint32_t exticr2;		// 0x0C
	uint32_t exticr3;		// 0x10
	uint32_t exticr4;		// 0x14
	uint32_t scsr;			// 0x18
	uint32_t cfgr2;			// 0x1C
	uint32_t swpr;			// 0x20
	uint32_t skr;			// 0x24
};

