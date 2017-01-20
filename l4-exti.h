#pragma once
#include <stdint.h>
#include <stddef.h>

struct exti_imr1 {
	uint32_t mr0 : 1, mr1 : 1, mr2 : 1, mr3 : 1, mr4 : 1, mr5 : 1, mr6 : 1,
		 mr7 : 1, mr8 : 1, mr9 : 1, mr10 : 1, mr11 : 1, mr12 : 1,
		 mr13 : 1, mr14 : 1, mr15 : 1, mr16 : 1, mr17 : 1, mr18 : 1,
		 mr19 : 1, mr20 : 1, mr21 : 1, mr22 : 1, mr23 : 1, mr24 : 1,
		 mr25 : 1, mr26 : 1, mr27 : 1, mr28 : 1, : 2, mr31 : 1;
};

struct exti_emr1 {
	uint32_t mr0 : 1, mr1 : 1, mr2 : 1, mr3 : 1, mr4 : 1, mr5 : 1, mr6 : 1,
		 mr7 : 1, mr8 : 1, mr9 : 1, mr10 : 1, mr11 : 1, mr12 : 1,
		 mr13 : 1, mr14 : 1, mr15 : 1, mr16 : 1, mr17 : 1, mr18 : 1,
		 mr19 : 1, mr20 : 1, mr21 : 1, mr22 : 1, mr23 : 1, mr24 : 1,
		 mr25 : 1, mr26 : 1, mr27 : 1, mr28 : 1, : 2, mr31 : 1;
};

struct exti_rtsr1 {
	uint32_t rt0 : 1, rt1 : 1, rt2 : 1, rt3 : 1, rt4 : 1, rt5 : 1, rt6 : 1,
		 rt7 : 1, rt8 : 1, rt9 : 1, rt10 : 1, rt11 : 1, rt12 : 1,
		 rt13 : 1, rt14 : 1, rt15 : 1, rt16 : 1, : 1, rt18 : 1,
		 rt19 : 1, rt20 : 1, rt21 : 1, rt22 : 1, : 9;
};

struct exti_ftsr1 {
	uint32_t ft0 : 1, ft1 : 1, ft2 : 1, ft3 : 1, ft4 : 1, ft5 : 1, ft6 : 1,
		 ft7 : 1, ft8 : 1, ft9 : 1, ft10 : 1, ft11 : 1, ft12 : 1,
		 ft13 : 1, ft14 : 1, ft15 : 1, ft16 : 1, : 1, ft18 : 1,
		 ft19 : 1, ft20 : 1, ft21 : 1, ft22 : 1, : 9;
};

struct exti_swier1 {
	uint32_t swi0 : 1, swi1 : 1, swi2 : 1, swi3 : 1, swi4 : 1, swi5 : 1,
		 swi6 : 1, swi7 : 1, swi8 : 1, swi9 : 1, swi10 : 1, swi11 : 1,
		 swi12 : 1, swi13 : 1, swi14 : 1, swi15 : 1, swi16 : 1, : 1,
		 swi18 : 1, swi19 : 1, swi20 : 1, swi21 : 1, swi22 : 1, : 9;
};

struct exti_pr1 {
	uint32_t pif0 : 1, pif1 : 1, pif2 : 1, pif3 : 1, pif4 : 1, pif5 : 1,
		 pif6 : 1, pif7 : 1, pif8 : 1, pif9 : 1, pif10 : 1, pif11 : 1,
		 pif12 : 1, pif13 : 1, pif14 : 1, pif15 : 1, pif16 : 1, : 1,
		 pif18 : 1, pif19 : 1, pif20 : 1, pif21 : 1, pif22 : 1, : 9;
};

struct exti_imr2 {
	uint32_t im32 : 1, im33 : 1, im34 : 1, im35 : 1, im36 : 1, im37 : 1,
		 im38 : 1, im39 : 1, : 24;
};

struct exti_emr2 {
	uint32_t em32 : 1, em33 : 1, em34 : 1, em35 : 1, em36 : 1, em37 : 1,
		 em38 : 1, em39 : 1, : 24;
};

struct exti_rtsr2 {
	uint32_t : 3, rt35 : 1, rt36 : 1, rt37 : 1, rt38 : 1, : 25;
};

struct exti_ftsr2 {
	uint32_t : 3, ft35 : 1, ft36 : 1, ft37 : 1, ft38 : 1, : 25;
};

struct exti_swier2 {
	uint32_t : 3, swi35 : 1, swi36 : 1, swi37 : 1, swi38 : 1, : 25;
};

struct exti_pr2 {
	uint32_t : 3, pif35 : 1, pif36 : 1, pif37 : 1, pif38 : 1, : 25;
};

struct exti_reg {
	struct exti_imr1 imr1;
	struct exti_emr1 emr1;
	struct exti_rtsr1 rtsr1;
	struct exti_ftsr1 ftsr1;
	struct exti_swier1 swier1;
	struct exti_pr1 pr1;		// 0x14
	uint32_t _padding0[2];		// 0x18 0x1C

	struct exti_imr2 imr2;		// 0x20
	struct exti_emr2 emr2;		// 0x24
	struct exti_rtsr2 rtsr2;	// 0x28
	struct exti_ftsr2 ftsr2;	// 0x28
	struct exti_swier2 swier2;	// 0x30
	struct exti_pr2 pr2;		// 0x34
};

_Static_assert (offsetof(struct exti_reg, pr2) == 0x34,
		"Exti registers don't add up.");
