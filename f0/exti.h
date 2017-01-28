#pragma once
#include <stdint.h>
#include <stddef.h>

struct exti_imr {
	uint32_t im0 : 1, im1 : 1, im2 : 1, im3 : 1, im4 : 1, im5 : 1, im6 : 1,
		 im7 : 1, im8 : 1, im9 : 1, im10 : 1, im11 : 1, im12 : 1,
		 im13 : 1, im14 : 1, im15 : 1, im16 : 1, im17 : 1, im18 : 1,
		 im19 : 1, im20 : 1, im21 : 1, im22 : 1, im23 : 1, im24 : 1,
		 im25 : 1, im26 : 1, im27 : 1, im28 : 1, im29 : 1, im30 : 1,
		 im31 : 1;
};

struct exti_emr {
	uint32_t em0 : 1, em1 : 1, em2 : 1, em3 : 1, em4 : 1, em5 : 1, em6 : 1,
		 em7 : 1, em8 : 1, em9 : 1, em10 : 1, em11 : 1, em12 : 1,
		 em13 : 1, em14 : 1, em15 : 1, em16 : 1, em17 : 1, em18 : 1,
		 em19 : 1, em20 : 1, em21 : 1, em22 : 1, em23 : 1, em24 : 1,
		 em25 : 1, em26 : 1, em27 : 1, em28 : 1, em29 : 1, em30 : 1,
		 em31 : 1;
};

struct exti_rtsr {
	uint32_t rt0 : 1, rt1 : 1, rt2 : 1, rt3 : 1, rt4 : 1, rt5 : 1, rt6 : 1,
		 rt7 : 1, rt8 : 1, rt9 : 1, rt10 : 1, rt11 : 1, rt12 : 1,
		 rt13 : 1, rt14 : 1, rt15 : 1, rt16 : 1, rt17 : 1, : 1,
		 rt19 : 1, rt20 : 1, rt21 : 1, rt22 : 1, : 9;
};

struct exti_ftsr {
	uint32_t tr0 : 1, tr1 : 1, tr2 : 1, tr3 : 1, tr4 : 1, tr5 : 1, tr6 : 1,
		 tr7 : 1, tr8 : 1, tr9 : 1, tr10 : 1, tr11 : 1, tr12 : 1,
		 tr13 : 1, tr14 : 1, tr15 : 1, tr16 : 1, tr17 : 1, : 1,
		 tr19 : 1, tr20 : 1, tr21 : 1, tr22 : 1, : 9;
};

struct exti_swier {
	uint32_t swier0 : 1, swier1 : 1, swier2 : 1, swier3 : 1, swier4 : 1,
		 swier5 : 1, swier6 : 1, swier7 : 1, swier8 : 1, swier9 : 1,
		 swier10 : 1, swier11 : 1, swier12 : 1, swier13 : 1,
		 swier14 : 1, swier15 : 1, swier16 : 1, swier17 : 1, : 1,
		 swier19 : 1, swier20 : 1, swier21 : 1, swier22 : 1, : 9;
};

struct exti_pr {
	uint32_t pif0 : 1, pif1 : 1, pif2 : 1, pif3 : 1, pif4 : 1, pif5 : 1, pif6 : 1,
		 pif7 : 1, pif8 : 1, pif9 : 1, pif10 : 1, pif11 : 1, pif12 : 1,
		 pif13 : 1, pif14 : 1, pif15 : 1, pif16 : 1, pif17 : 1, : 1,
		 pif19 : 1, pif20 : 1, pif21 : 1, pif22 : 1, : 9;
};

struct exti_reg {
	struct exti_imr imr;
	struct exti_emr emr;
	struct exti_rtsr rtsr;
	struct exti_ftsr ftsr;
	struct exti_swier swier;
	struct exti_pr pr;
};

_Static_assert (offsetof(struct exti_reg, pr) == 0x14,
		"Exti registers don't add up.");
