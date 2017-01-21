#pragma once
#include <stdint.h>
#include <stddef.h>

struct pwr_cr {
	uint32_t lpsdsr : 1, pdds : 1, cwuf : 1, csbf : 1, pvde : 1, pls : 3,
		 dbp : 1, ulp : 1, fwu : 1, vos : 2, ds_ee_koff : 1, lprun : 1,
		 : 1, lpds : 1, : 15;
};

struct pwr_csr {
	uint32_t wuf : 1, sbf : 1, pvdo : 1, vrefintrdyf : 1, vosf : 1,
		 reglpf : 1, : 2, ewup1 : 1, ewup2 : 1, ewup3 : 1, : 21;
};

struct pwr_reg {
	struct pwr_cr cr;		// 0x00
	struct pwr_csr csr;		// 0x04
};

_Static_assert (offsetof(struct pwr_reg, csr) == 0x04,
		"PWR registers size mismatch.");

