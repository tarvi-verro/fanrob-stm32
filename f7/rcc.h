#pragma once
#include <stdint.h>
#include <stddef.h>

struct rcc_ahb1enr {
	uint32_t gpioaen : 1, gpioben : 1, gpiocen : 1, gpioden : 1,
		 gpioeen : 1, gpiofen : 1, gpiogen : 1, gpiohen : 1,
		 gpioien : 1, gpiojen : 1, gpioken : 1, : 1, crcen : 1, : 5,
		 bkpsramen : 1, : 1, dtcmramen : 1, dma1en : 1, dma2en : 1,
		 dma2den : 1, : 1, ethmacen : 1, ethmactxen : 1,
		 ethmacrxen : 1, ethmacptpen : 1, otghsen : 1, otghsulpien : 1,
		 : 1;
};

struct rcc_reg {
	uint32_t cr;			// 0x00
	uint32_t pllcfgr;		// 0x04
	uint32_t cfgr;			// 0x08
	uint32_t cir;			// 0x0C
	uint32_t ahb1rstr;		// 0x10
	uint32_t ahb2rstr;		// 0x14
	uint32_t ahb3rstr;		// 0x18
	uint32_t _padding0[1];		// 0x1C
	uint32_t apb1rstr;		// 0x20
	uint32_t apb2rstr;		// 0x24
	uint32_t _padding1[2];		// 0x28
					// 0x2C
	struct rcc_ahb1enr ahb1enr;	// 0x30
	uint32_t ahb2enr;		// 0x34
	uint32_t ahb3enr;		// 0x38
	uint32_t _padding2[1];		// 0x3C
	uint32_t apb1enr;		// 0x40
	uint32_t apb2enr;		// 0x44
	uint32_t _padding3[2];		// 0x48
					// 0x4C
	uint32_t ahb1lpenr;		// 0x50
	uint32_t ahb2lpenr;		// 0x54
	uint32_t ahb3lpenr;		// 0x58
	uint32_t _padding4[1];		// 0x5C
	uint32_t apb1lpenr;		// 0x60
	uint32_t apb2lpenr;		// 0x64
	uint32_t _padding5[2];		// 0x68
					// 0x6C
	uint32_t bdcr;			// 0x70
	uint32_t csr;			// 0x74
	uint32_t _padding6[2];		// 0x78
					// 0x7C
	uint32_t sscgr;			// 0x80
	uint32_t plli2scfgr;		// 0x84
	uint32_t pllsaicfgr;		// 0x88
	uint32_t dckcfgr1;		// 0x8C
	uint32_t dckcfgr2;		// 0x90
};

_Static_assert (offsetof(struct rcc_reg, dckcfgr2) == 0x90,
		"RCC register boundary mismatch.");

