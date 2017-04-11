#pragma once
#include <stdint.h>
#include <stddef.h>

union dma_isr {
	struct { uint32_t :  0, gif : 1, tcif : 1, htif : 1, teif : 1, : 28; }
	ch1;
	struct { uint32_t :  4, gif : 1, tcif : 1, htif : 1, teif : 1, : 24; }
	ch2;
	struct { uint32_t :  8, gif : 1, tcif : 1, htif : 1, teif : 1, : 20; }
	ch3;
	struct { uint32_t : 12, gif : 1, tcif : 1, htif : 1, teif : 1, : 16; }
	ch4;
	struct { uint32_t : 16, gif : 1, tcif : 1, htif : 1, teif : 1, : 12; }
	ch5;
	struct { uint32_t : 20, gif : 1, tcif : 1, htif : 1, teif : 1, :  8; }
	ch6;
	struct { uint32_t : 24, gif : 1, tcif : 1, htif : 1, teif : 1, :  4; }
	ch7;
};

union dma_ifcr {
	struct { uint32_t :  0, cgif : 1, ctcif : 1, chtif : 1, cteif : 1,
		: 28; } ch1;
	struct { uint32_t :  4, cgif : 1, ctcif : 1, chtif : 1, cteif : 1,
		: 24; } ch2;
	struct { uint32_t :  8, cgif : 1, ctcif : 1, chtif : 1, cteif : 1,
		: 20; } ch3;
	struct { uint32_t : 12, cgif : 1, ctcif : 1, chtif : 1, cteif : 1,
		: 16; } ch4;
	struct { uint32_t : 16, cgif : 1, ctcif : 1, chtif : 1, cteif : 1,
		: 12; } ch5;
	struct { uint32_t : 20, cgif : 1, ctcif : 1, chtif : 1, cteif : 1,
		:  8; } ch6;
	struct { uint32_t : 24, cgif : 1, ctcif : 1, chtif : 1, cteif : 1,
		:  4; } ch7;
};

enum dma_psize {
	DMA_PSIZE_8, DMA_PSIZE_16, DMA_PSIZE_32,
};
enum dma_msize {
	DMA_MSIZE_8 = 0, DMA_MSIZE_16, DMA_MSIZE_32,
};
enum dma_dir {
	DMA_DIR_FROM_PERIP, DMA_DIR_FROM_MEM,
};
enum dma_pl {
	DMA_PL_LOW,
	DMA_PL_MEDIUM,
	DMA_PL_HIGH,
	DMA_PL_VERY_HIGH,
};
struct dma_ccr {
	uint32_t en : 1, tcie : 1, htie : 1, teie : 1;
	enum dma_dir dir : 1;
	uint32_t circ : 1, pinc : 1, minc : 1;
	enum dma_psize psize : 2;
	enum dma_msize msize : 2;
	enum dma_pl pl : 2;
	uint32_t mem2mem : 1, : 1;
	uint32_t : 16;
};

struct dma_cndtr {
	uint32_t ndt : 16, : 16;
};

union dma_cselr {
	struct { uint32_t :  0, cs : 4, : 28; } ch1;
	struct { uint32_t :  4, cs : 4, : 24; } ch2;
	struct { uint32_t :  8, cs : 4, : 20; } ch3;
	struct { uint32_t : 12, cs : 4, : 16; } ch4;
	struct { uint32_t : 16, cs : 4, : 12; } ch5;
	struct { uint32_t : 20, cs : 4, :  8; } ch6;
	struct { uint32_t : 24, cs : 4, :  4; } ch7;
};
_Static_assert(sizeof(union dma_cselr) == 0x4,
		"DMA cselr register size wrong.");

struct dma_ch {
	union { struct dma_ccr ccr; uint32_t ccr32; };
	struct dma_cndtr cndtr; /* dma_cndtr */
	uint32_t cpar;
	uint32_t cmar;
	uint32_t _res2;
};
_Static_assert (sizeof(void *) == 0x4, "Wrong size for pointers!");
_Static_assert (sizeof(struct dma_ch) == (0x1c - 0x8),
		"DMA channel size wrong.");

struct dma_reg {
	union dma_isr isr;
	union dma_ifcr ifcr;
	struct dma_ch ch1;
	struct dma_ch ch2;
	struct dma_ch ch3;
	struct dma_ch ch4;
	struct dma_ch ch5;
	struct dma_ch ch6;
	struct dma_ch ch7;
	uint32_t _res1[5];
	union dma_cselr cselr;
};

_Static_assert (offsetof(struct dma_reg, ch7) == 0x80,
		"DMA registers haven't even done channels properly.");

_Static_assert (offsetof(struct dma_reg, cselr) == 0xa8,
		"DMA registers have taken a misstep.");
