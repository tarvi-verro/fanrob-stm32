#pragma once
#include <stdint.h>
#include <stddef.h>

struct usart_cr1 {
	uint32_t ue : 1, uesm : 1, re : 1, te : 1, idleie : 1, rxneie : 1,
		 tcie : 1, txeie : 1, peie : 1, ps : 1, pce : 1, wake : 1,
		 m0 : 1, mme : 1, cmie : 1, over8 : 1, dedt : 5, deat : 5,
		 rtoie : 1, eobie : 1, m1 : 1, : 3;
};

struct usart_cr2 {
	uint32_t : 4, addm7 : 1, lbdl : 1, lbdie : 1, : 1, lbcl : 1, cpha : 1,
		 cpol : 1, clken : 1, stop : 2, linen : 1, swap : 1, rxinv : 1,
		 txinv : 1, datainv : 1, msbfirst : 1, abren : 1, abrmod : 2,
		 rtoen : 1, add_0 : 4, add_1 : 4;
};

struct usart_cr3 {
	uint32_t eie : 1, iren : 1, irlp : 1, hdsel : 1, nack : 1, scen : 1,
		 dmar : 1, dmat : 1, rtse : 1, ctse : 1, ctsie : 1, onebit : 1,
		 ovrdis : 1, ddre : 1, dem : 1, dep : 1, : 1, scarcnt : 3,
		 wus : 2, wufie : 1, ucesm : 1, tcbgtie : 1, : 7;
};

struct usart_brr {
	uint32_t brr : 16, : 16;
};

struct usart_gtpr {
	uint32_t psc : 8, gt : 8, : 16;
};

struct usart_rtor {
	uint32_t rto : 24, blen : 8;
};

struct usart_rqr {
	uint32_t abrrq : 1, sbkrq : 1, mmrq : 1, rxfrq : 1, txfrq : 1, : 27;
};

struct usart_isr {
	uint32_t pe : 1, fe : 1, nf : 1, ore : 1, idle : 1, rxne : 1, tc : 1,
		 txe : 1, lbdf : 1, ctsif : 1, cts : 1, rtof : 1, eobf : 1,
		 : 1, abre : 1, abrf : 1, busy : 1, cmf : 1, sbkf : 1, rwu : 1,
		 wuf : 1, teack : 1, reack : 1, : 2, tcbgt : 1, : 6;
};

struct usart_icr {
	uint32_t pecf : 1, fecf : 1, ncf : 1, orecf : 1, idlecf : 1, : 1,
		 tccf : 1, tcbgtcf : 1, lbdcf : 1, ctscf : 1, : 1, rtocf : 1,
		 eobcf : 1, : 4, cmcf : 1, : 2, wucf : 1, : 11;
};

struct usart_rdr {
	uint32_t rdr : 9, : 23;
};

struct usart_tdr {
	uint32_t tdr : 9, : 23;
};

struct usart_reg {
	struct usart_cr1 cr1;		// 0x00
	struct usart_cr2 cr2;		// 0x04
	struct usart_cr3 cr3;		// 0x08
	struct usart_brr brr;		// 0x0C
	struct usart_gtpr gtpr;		// 0x10
	struct usart_rtor rtor;		// 0x14
	struct usart_rqr rqr;		// 0x18
	struct usart_isr isr;		// 0x1C
	struct usart_icr icr;		// 0x20
	struct usart_rdr rdr;		// 0x24
	struct usart_tdr tdr;		// 0x28
};

_Static_assert(offsetof(struct usart_reg, tdr) == 0x28,
		"Usart registers mismatch.");
