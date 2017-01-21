#pragma once
#include <stdint.h>
#include <stddef.h>

enum {
	LPUART_PARITY_EVEN,
	LPUART_PARITY_ODD,
};

enum {
	LPUART_WAKE_IDLE,
	LPUART_WAKE_ADDRESS_MARK,
};

struct lpuart_cr1 {
	uint32_t ue : 1, uesm : 1, re : 1, te : 1, idleie : 1, rxneie : 1,
		 tcie : 1, txeie : 1, peie : 1, ps : 1, pce : 1, wake : 1,
		 m0 : 1, mme : 1, cmie : 1, : 1, dedt : 5, deat : 5, : 2,
		 m1 : 1, : 3;
};

struct lpuart_cr2 {
	uint32_t : 4, addm7 : 1, : 7, stop : 2, : 1, swap : 1, rxinv : 1,
		 txinv : 1, datainv : 1, msbfirst : 1, : 4, add_0 : 4,
		 add_1 : 4;
};

struct lpuart_cr3 {
	uint32_t eie : 1, : 2, hdsel : 1, : 2, dmar : 1, dmat : 1, rtse : 1,
		 ctse : 1, ctsie : 1, : 1, ovrdis : 1, ddre : 1, dem : 1,
		 dep : 1, : 4, wus : 2, wufie : 1, ucesm : 1, : 8;
};

struct lpuart_brr {
	uint32_t brr : 20, : 12;
};

struct lpuart_rdr {
	uint32_t rdr : 9, : 23;
};

struct lpuart_tdr {
	uint32_t tdr : 9, : 23;
};

struct lpuart_isr {
	uint32_t pe : 1, fe : 1, nf : 1, ore : 1, idle : 1, rxne : 1, tc : 1,
		 txe : 1, : 1, ctsif : 1, cts : 1, : 5, busy : 1, cmf : 1,
		 sbkf : 1, rwu : 1, wuf : 1, teack : 1, reack : 1, : 9;
};

struct lpuart_icr {
	uint32_t pecf : 1, fecf : 1, ncf : 1, orecf : 1, idlecf : 1, : 1,
		 tccf : 1, : 2, ctscf : 1, : 7, cmcf : 1, : 2, wucf : 1, : 11;
};

struct lpuart_reg {
	struct lpuart_cr1 cr1;	// 0x00
	struct lpuart_cr2 cr2;	// 0x04
	struct lpuart_cr3 cr3;	// 0x08
	struct lpuart_brr brr;	// 0x0C
	uint32_t _padding0[2];	// 0x10 0x14
	uint32_t rqr;		// 0x18
	struct lpuart_isr isr;	// 0x1C
	struct lpuart_icr icr;	// 0x20
	struct lpuart_rdr rdr;	// 0x24
	struct lpuart_tdr tdr;	// 0x28
};

_Static_assert (offsetof(struct lpuart_reg, tdr) == 0x28,
		"Lpuart registers don't add up.");
