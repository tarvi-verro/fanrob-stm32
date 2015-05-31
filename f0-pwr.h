#include <stdint.h>
#include <stddef.h>

struct pwr_cr {
	uint32_t lpds : 1, pdds : 1, cwuf : 1, csbf : 1, : 4, dbp : 1, : 23;
};

struct pwr_reg {
	struct pwr_cr cr;
	uint32_t csr;
};

_Static_assert (offsetof(struct pwr_reg, csr) == 0x4,
		"PWR registers size mismatch.");
