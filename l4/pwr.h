#pragma once
#include <stdint.h>
#include <stddef.h>

struct pwr_cr1 {
	uint32_t lpms : 3, : 5, dbp : 1, vos : 2, : 3, lpr : 1, : 17;
};

struct pwr_cr2 {
	uint32_t pvde : 1, pls : 3, pvme1 : 1, : 1, pvme3 : 1, pvme4 : 1, : 2,
		 usv : 1, : 21;
};

struct pwr_cr3 {
	uint32_t ewup1 : 1, ewup2 : 1, ewup3 : 1, ewup4 : 1, ewup5 : 1, : 3,
		 rrs : 1, : 1, apc : 1, : 4, eiwul : 1, : 16;
};

struct pwr_cr4 {
	uint32_t wp1 : 1, wp2 : 1, wp3 : 1, wp4 : 1, wp5 : 1, : 3, vbe : 1,
		 vbrs : 1, : 22;
};

struct pwr_sr1 {
	uint32_t wuf1 : 1, wuf2 : 1, wuf3 : 1, wuf4 : 1, wuf5 : 1, : 3,
		 sbf : 1, : 6, wufi : 1, : 16;
};

struct pwr_sr2 {
	uint32_t : 8, reglps : 1, reglpf : 1, vosf : 1, pvdo : 1, pvmo1 : 1,
		 : 1, pvmo3 : 1, pvmo4 : 1, : 16;
};

struct pwr_scr {
	uint32_t cwuf1 : 1, cwuf2 : 1, cwuf3 : 1, cwuf4 : 1, cwuf5 : 1, : 3,
		 csbf : 1, : 23;
};

/* Power Port A pull up/down */
struct pwr_pucra {
	uint32_t pu0 : 1, pu1 : 1, pu2 : 1, pu3 : 1, pu4 : 1, pu5 : 1, pu6 : 1,
		 pu7 : 1, pu8 : 1, pu9 : 1, pu10 : 1, pu11 : 1, pu12 : 1,
		 pu13 : 1, : 1, pu15 : 1, : 16;
};

struct pwr_pdcra {
	uint32_t pd0 : 1, pd1 : 1, pd2 : 1, pd3 : 1, pd4 : 1, pd5 : 1, pd6 : 1,
		 pd7 : 1, pd8 : 1, pd9 : 1, pd10 : 1, pd11 : 1, pd12 : 1, : 1,
		 pd14 : 1, : 17;
};

/* Power Port B pull up/down */
struct pwr_pucrb {
	uint32_t pu0 : 1, pu1 : 1, pu2 : 1, pu3 : 1, pu4 : 1, pu5 : 1, pu6 : 1,
		 pu7 : 1, pu8 : 1, pu9 : 1, pu10 : 1, pu11 : 1, pu12 : 1,
		 pu13 : 1, pu14 : 1, pu15 : 1, : 16;
};

struct pwr_pdcrb {
	uint32_t pd0 : 1, pd1 : 1, pd2 : 1, pd3 : 1, : 1, pd5 : 1, pd6 : 1,
		 pd7 : 1, pd8 : 1, pd9 : 1, pd10 : 1, pd11 : 1, pd12 : 1,
		 pd13 : 1, pd14 : 1, pd15 : 1, : 16;
};

/* Power Port C pull up/down */
struct pwr_pucrc {
	uint32_t pu0 : 1, pu1 : 1, pu2 : 1, pu3 : 1, pu4 : 1, pu5 : 1, pu6 : 1,
		 pu7 : 1, pu8 : 1, pu9 : 1, pu10 : 1, pu11 : 1, pu12 : 1,
		 pu13 : 1, pu14 : 1, pu15 : 1, : 16;
};

struct pwr_pdcrc {
	uint32_t pd0 : 1, pd1 : 1, pd2 : 1, pd3 : 1, pd4 : 1, pd5 : 1, pd6 : 1,
		 pd7 : 1, pd8 : 1, pd9 : 1, pd10 : 1, pd11 : 1, pd12 : 1,
		 pd13 : 1, pd14 : 1, pd15 : 1, : 16;
};

/* Power Port D pull up/down */
struct pwr_pucrd {
	uint32_t pu0 : 1, pu1 : 1, pu2 : 1, pu3 : 1, pu4 : 1, pu5 : 1, pu6 : 1,
		 pu7 : 1, pu8 : 1, pu9 : 1, pu10 : 1, pu11 : 1, pu12 : 1,
		 pu13 : 1, pu14 : 1, pu15 : 1, : 16;
};

struct pwr_pdcrd {
	uint32_t pd0 : 1, pd1 : 1, pd2 : 1, pd3 : 1, pd4 : 1, pd5 : 1, pd6 : 1,
		 pd7 : 1, pd8 : 1, pd9 : 1, pd10 : 1, pd11 : 1, pd12 : 1,
		 pd13 : 1, pd14 : 1, pd15 : 1, : 16;
};

/* Power Port E pull up/down */
struct pwr_pucre {
	uint32_t pu0 : 1, pu1 : 1, pu2 : 1, pu3 : 1, pu4 : 1, pu5 : 1, pu6 : 1,
		 pu7 : 1, pu8 : 1, pu9 : 1, pu10 : 1, pu11 : 1, pu12 : 1,
		 pu13 : 1, pu14 : 1, pu15 : 1, : 16;
};

struct pwr_pdcre {
	uint32_t pd0 : 1, pd1 : 1, pd2 : 1, pd3 : 1, pd4 : 1, pd5 : 1, pd6 : 1,
		 pd7 : 1, pd8 : 1, pd9 : 1, pd10 : 1, pd11 : 1, pd12 : 1,
		 pd13 : 1, pd14 : 1, pd15 : 1, : 16;
};

/* Power Port H pull up/down */
struct pwr_pucrh {
	uint32_t pu0 : 1, pu1 : 1, : 1, pu3 : 1, : 28;
};

struct pwr_pdcrh {
	uint32_t pd0 : 1, pd1 : 1, : 1, pd3 : 1, : 28;
};

struct pwr_reg {
	struct pwr_cr1 cr1;		// 0x00
	struct pwr_cr2 cr2;		// 0x04
	struct pwr_cr3 cr3;		// 0x08
	struct pwr_cr4 cr4;		// 0x0C

	struct pwr_sr1 sr1;		// 0x10
	struct pwr_sr2 sr2;		// 0x14
	struct pwr_scr scr;		// 0x18
	uint32_t _padding0[1];		// 0x1C

	struct pwr_pucra pucra;		// 0x20
	struct pwr_pdcra pdcra;		// 0x24
	struct pwr_pucrb pucrb;		// 0x28
	struct pwr_pdcrb pdcrb;		// 0x2C

	struct pwr_pucrc pucrc;		// 0x30
	struct pwr_pdcrc pdcrc;		// 0x34
	struct pwr_pucrd pucrd;		// 0x38
	struct pwr_pdcrd pdcrd;		// 0x3C

	struct pwr_pucre pucre;		// 0x40
	struct pwr_pdcre pdcre;		// 0x44

	uint32_t _padding1[4];		// 0x48 0x4C 0x50 0x54

	struct pwr_pucrh pucrh;		// 0x58
	struct pwr_pdcrh pdcrh;		// 0x5C
};

_Static_assert (offsetof(struct pwr_reg, pdcrh) == 0x5C,
		"PWR registers size mismatch.");

