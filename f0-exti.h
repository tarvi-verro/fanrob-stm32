#pragma once
#include <stdint.h>
#include <stddef.h>

struct exti_imr {
	uint32_t mr0 : 1, mr1 : 1, mr2 : 1, mr3 : 1, mr4 : 1, mr5 : 1, mr6 : 1,
		 mr7 : 1, mr8 : 1, mr9 : 1, mr10 : 1, mr11 : 1, mr12 : 1,
		 mr13 : 1, mr14 : 1, mr15 : 1, mr16 : 1, mr17 : 1, mr18 : 1,
		 mr19 : 1, mr20 : 1, mr21 : 1, mr22 : 1, mr23 : 1, mr24 : 1,
		 mr25 : 1, mr26 : 1, mr27 : 1, mr28 : 1, mr29 : 1, mr30 : 1,
		 mr31 : 1;
};

struct exti_emr {
	uint32_t mr0 : 1, mr1 : 1, mr2 : 1, mr3 : 1, mr4 : 1, mr5 : 1, mr6 : 1,
		 mr7 : 1, mr8 : 1, mr9 : 1, mr10 : 1, mr11 : 1, mr12 : 1,
		 mr13 : 1, mr14 : 1, mr15 : 1, mr16 : 1, mr17 : 1, mr18 : 1,
		 mr19 : 1, mr20 : 1, mr21 : 1, mr22 : 1, mr23 : 1, mr24 : 1,
		 mr25 : 1, mr26 : 1, mr27 : 1, mr28 : 1, mr29 : 1, mr30 : 1,
		 mr31 : 1;
};

struct exti_rtsr {
	uint32_t tr0 : 1, tr1 : 1, tr2 : 1, tr3 : 1, tr4 : 1, tr5 : 1, tr6 : 1,
		 tr7 : 1, tr8 : 1, tr9 : 1, tr10 : 1, tr11 : 1, tr12 : 1,
		 tr13 : 1, tr14 : 1, tr15 : 1, tr16 : 1, tr17 : 1, : 1,
		 tr19 : 1, tr20 : 1, tr21 : 1, tr22 : 1, : 9;
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
	uint32_t pr0 : 1, pr1 : 1, pr2 : 1, pr3 : 1, pr4 : 1, pr5 : 1, pr6 : 1,
		 pr7 : 1, pr8 : 1, pr9 : 1, pr10 : 1, pr11 : 1, pr12 : 1,
		 pr13 : 1, pr14 : 1, pr15 : 1, pr16 : 1, pr17 : 1, : 1,
		 pr19 : 1, pr20 : 1, pr21 : 1, pr22 : 1, : 9;
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
