#pragma once
#define _EXTI_ABS_H
#include "exti.h"
#include "gpio-abs.h"

__attribute__ ((__packed__))  enum line {
	PINE0, PINE1, PINE2, PINE3, PINE4, PINE5, PINE6, PINE7,
	PINE8, PINE9, PINE10, PINE11, PINE12, PINE13, PINE14, PINE15,

	LINE16, LINE17, LINE18, LINE19, LINE20, LINE21, LINE22, LINE23,
	LINE24, LINE25, LINE26, LINE27, LINE28, __NE29, __NE30, LINE31,

	LINE32, LINE33, LINE34, LINE35, LINE36, LINE37, LINE38, LINE39,

	LNONE,

	LINE_RTC_WUT = LINE20,
	LINE_RTC_ALR = LINE18,
};

struct exti_conf {
	uint8_t im : 1, // interrupt mask
		em : 1, // event mask
		rt : 1, // rising trigger
		ft : 1, // falling trigger
		swi : 1; // software interrupt
};

extern void exti_configure_pin(enum pin p, struct exti_conf *cfg, void (*const *exti_callb)(enum pin));

static inline void exti_pending_clear(enum line l)
{
	if (l == LNONE) return;
	uint32_t *pr = (uint32_t *) (l / 32 ? &exti->pr2 : &exti->pr1);
	uint32_t b = 1 << l;
	*pr |= b;
}

static inline void exti_configure_line(enum line l, struct exti_conf *cfg)
{
	if (l == LNONE) return;
	uint32_t *im;
	uint32_t *em;
	uint32_t *rt;
	uint32_t *ft;
	uint32_t *swi;
	uint32_t *pr;

	if (l / 32) {
		im  = (uint32_t *) &exti->imr2;
		em = (uint32_t *) &exti->emr2;
		rt = (uint32_t *) &exti->rtsr2;
		ft = (uint32_t *) &exti->ftsr2;
		swi = (uint32_t *) &exti->swier2;
		pr = (uint32_t *) &exti->pr2;
	} else {
		im  = (uint32_t *) &exti->imr1;
		em = (uint32_t *) &exti->emr1;
		rt = (uint32_t *) &exti->rtsr1;
		ft = (uint32_t *) &exti->ftsr1;
		swi = (uint32_t *) &exti->swier1;
		pr = (uint32_t *) &exti->pr1;
	}

	uint32_t b = 1 << (l % 32);
	*em &= ~b;
	*em |= b * cfg->em;

	*rt &= ~b;
	*rt |= b * cfg->rt;

	*ft &= ~b;
	*ft |= b * cfg->ft;

	*swi &= ~b;
	*swi |= b * cfg->swi;

	*im &= ~b;
	*im |= b * cfg->im;

	*pr |= b;
}
