#pragma once
#include <stdint.h>
#include <stddef.h>

struct usart_sr {
	uint32_t pe : 1, fe : 1, ne : 1, ore : 1, idle : 1, rxne : 1, tc : 1,
		 txe : 1, lbd : 1, cts : 1,  : 6, : 16;
};

struct usart_dr {
	uint32_t dr : 9, : 23;
};

struct usart_brr {
	uint32_t div_fraction : 4, div_mantissa : 12, : 16;
};

enum usart_m {
	USART_WORD_LENGTH_8,
	USART_WORD_LENGTH_9,
};
struct usart_cr1 {
	uint32_t sbk : 1, rwu : 1, re : 1, te : 1, idleie : 1, rxneie : 1,
		 tcie : 1, txeie :1 , peie : 1, ps : 1, pce : 1, wake : 1;
	enum usart_m m : 1;
	uint32_t ue : 1, : 2, : 16;
};

struct usart_cr2 {
	uint32_t add : 4, : 1, lbdl : 1, lbdie : 1, : 1, lbcl : 1, cpha : 1,
		 cpol : 1, clken : 1, stop : 2, linen : 1, : 1, : 16;
};

struct usart_cr3 {
	uint32_t eie : 1, iren : 1, irlp : 1, hdsel : 1, nack : 1, scen : 1,
		 dmar : 1, dmat : 1, rtse : 1, ctse : 1, ctsie : 1, : 5, : 16;
};

struct usart_gtpr {
	uint32_t psc : 8, gt : 8, : 16;
};

struct usart_reg {
	struct usart_sr sr;
	struct usart_dr dr;
	struct usart_brr brr;
	struct usart_cr1 cr1;
	struct usart_cr2 cr2;
	struct usart_cr3 cr3;
	struct usart_gtpr gtpr;
};
_Static_assert(offsetof(struct usart_reg, gtpr) == 0x18,
		"struct usart_reg misaligned");

