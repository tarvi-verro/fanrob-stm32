#pragma once
#include <stdint.h>
#include <stddef.h>

union dma_isr {
	struct { uint32_t gif : 1, tcif : 1, htif : 1, teif : 1,
		: 28; } ch1;
	struct { uint32_t : 4, gif : 1, tcif : 1, htif : 1, teif : 1,
		: 24; } ch2;
	struct { uint32_t : 8, gif : 1, tcif : 1, htif : 1, teif : 1,
		: 20; } ch3;
	struct { uint32_t : 12, gif : 1, tcif : 1, htif : 1, teif : 1,
		: 16; } ch4;
	struct { uint32_t : 16, gif : 1, tcif : 1, htif : 1, teif : 1,
		: 12; } ch5;
};

union dma_ifcr {
	struct { uint32_t cgif : 1, ctcif : 1, chtif : 1, cteif : 1,
		: 28; } ch1;
	struct { uint32_t : 4, cgif : 1, ctcif : 1, chtif : 1, cteif : 1,
		: 24; } ch2;
	struct { uint32_t : 8, cgif : 1, ctcif : 1, chtif : 1, cteif : 1,
		: 20; } ch3;
	struct { uint32_t : 12, cgif : 1, ctcif : 1, chtif : 1, cteif : 1,
		: 16; } ch4;
	struct { uint32_t : 16, cgif : 1, ctcif : 1, chtif : 1, cteif : 1,
		: 12; } ch5;
};

enum {
	DMA_PSIZE_8, DMA_PSIZE_16, DMA_PSIZE_32, DMA_MSIZE_8 = 0, DMA_MSIZE_16,
	DMA_MSIZE_32,
};
enum {
	DMA_DIR_FROM_PERIP, DMA_DIR_FROM_MEM,
};
struct dma_ccr {
	uint32_t en : 1, tcie : 1, htie : 1, teie : 1, dir : 1, circ : 1,
		 pinc : 1, minc : 1, psize : 2, msize : 2, pl : 2,
		 mem2mem : 1, : 17;
};

struct dma_cndtr {
	uint32_t ndt : 16, : 16;
};

struct dma_ch {
	struct dma_ccr ccr;
	struct dma_cndtr cndtr; /* dma_cndtr */
	//uint16_t _res;
	volatile void *cpar; /* dma_cpar */
	volatile void *cmar; /* dma_cmar */
	uint32_t _res2;
};
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
};

_Static_assert (offsetof(struct dma_reg, ch5) == 0x58,
		"DMA registers have taken a misstep.");
