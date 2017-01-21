#pragma once
#include <stdint.h>
#include <stddef.h>

struct exti_imr {
	uint32_t im0 : 1, im1 : 1, im2 : 1, im3 : 1, im4 : 1, im5 : 1, im6 : 1,
		 im7 : 1, im8 : 1, im9 : 1, im10 : 1, im11 : 1, im12 : 1,
		 im13 : 1, im14 : 1, im15 : 1, im16 : 1, im17 : 1, im18 : 1,
		 im19 : 1, im20 : 1, im21 : 1, im22 : 1, im23 : 1, im24 : 1,
		 im25 : 1, im26 : 1, : 1, im28 : 1, im29 : 1, : 2;
};

struct exti_emr {
	uint32_t em0 : 1, em1 : 1, em2 : 1, em3 : 1, em4 : 1, em5 : 1, em6 : 1,
		 em7 : 1, em8 : 1, em9 : 1, em10 : 1, em11 : 1, em12 : 1,
		 em13 : 1, em14 : 1, em15 : 1, em16 : 1, em17 : 1, em18 : 1,
		 em19 : 1, em20 : 1, em21 : 1, em22 : 1, em23 : 1, em24 : 1,
		 em25 : 1, em26 : 1, : 1, em28 : 1, em29 : 1, : 2;
};

struct exti_rtsr {
	uint32_t rt0 : 1, rt1 : 1, rt2 : 1, rt3 : 1, rt4 : 1, rt5 : 1, rt6 : 1,
		 rt7 : 1, rt8 : 1, rt9 : 1, rt10 : 1, rt11 : 1, rt12 : 1,
		 rt13 : 1, rt14 : 1, rt15 : 1, rt16 : 1, rt17 : 1, : 1,
		 rt19 : 1, rt20 : 1, rt21 : 1, rt22 : 1, : 9;
};

struct exti_ftsr {
	uint32_t ft0 : 1, ft1 : 1, ft2 : 1, ft3 : 1, ft4 : 1, ft5 : 1, ft6 : 1,
		 ft7 : 1, ft8 : 1, ft9 : 1, ft10 : 1, ft11 : 1, ft12 : 1,
		 ft13 : 1, ft14 : 1, ft15 : 1, ft16 : 1, ft17 : 1, : 1,
		 ft19 : 1, ft20 : 1, ft21 : 1, ft22 : 1, : 9;
};

struct exti_swier {
	uint32_t swi0 : 1, swi1 : 1, swi2 : 1, swi3 : 1, swi4 : 1, swi5 : 1,
		 swi6 : 1, swi7 : 1, swi8 : 1, swi9 : 1, swi10 : 1, swi11 : 1,
		 swi12 : 1, swi13 : 1, swi14 : 1, swi15 : 1, swi16 : 1,
		 swi17 : 1, : 1, swi19 : 1, swi20 : 1, swi21 : 1, swi22 : 1,
		 : 9;
};

struct exti_pr {
	uint32_t pif0 : 1, pif1 : 1, pif2 : 1, pif3 : 1, pif4 : 1, pif5 : 1,
		 pif6 : 1, pif7 : 1, pif8 : 1, pif9 : 1, pif10 : 1, pif11 : 1,
		 pif12 : 1, pif13 : 1, pif14 : 1, pif15 : 1, pif16 : 1,
		 pif17 : 1, : 1, pif19 : 1, pif20 : 1, pif21 : 1, pif22 : 1,
		 : 9;
};

struct exti_reg {
	struct exti_imr imr;		// 0x00
	struct exti_emr emr;		// 0x04
	struct exti_rtsr rtsr;		// 0x08
	struct exti_ftsr ftsr;		// 0x0C
	struct exti_swier swier;	// 0x10
	struct exti_pr pr;		// 0x14
};

_Static_assert (offsetof(struct exti_reg, pr) == 0x14,
		"Exti registers don't add up.");
