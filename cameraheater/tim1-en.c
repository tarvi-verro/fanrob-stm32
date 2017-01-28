#include "conf.h"

#ifndef CONF_F0
#warning tim1-en not supported on target
void setup_tim14() {}
void setup_tim3() {}
void setup_tim1() {}
#else

#include "tim.h"
#include "rcc.h"

void setup_tim1()
{
	if (tim1->cr1.cen)
		return;

	rcc->apb2enr.tim1en = 1;

	/* Setup tim1 */
	tim1->arr = 2550; /* auto-reload aka period */
	tim1->psc = 32000 - 1; /* prescaler */

//	tim1->cr1.arpe = 1; /* auto-preload enable */
	tim1->cr1.dir = TIM_DIR_UPCNT; /* upcounter */
//	tim1->ccer.cc1p = 0; /* polarity */
//	tim1->ccer.cc1ne = 0;
//	tim1->egr.cc1g = 1; /* capture/compare generation */
	tim1->egr.ug = 1; /* update generation */
	tim1->bdtr.moe = 1; /* main output enable */
	tim1->cr1.cen = 1; /* enable */
}

void setup_tim3()
{
	if (tim3->cr1.cen)
		return;

	rcc->apb1enr.tim3en = 1;
	/* Setup tim3 */
	tim3->arr = 2550; /* auto-reload aka period */
	//tim3->cnt = 500;
	tim3->psc = 32 - 1; /* prescaler */
	tim3->ccmr.ch_lcdbg.out.ccs = TIM_CCS_OUT; /* output */
	tim3->ccmr.ch_lcdbg.out.ocm = 6; /* 110 -- pwm mode 1 */
	tim3->ccmr.ch_lcdbg.out.ocpe = 1; /* preload enable, enables changing ccr1
					on the fly */
//	tim3->cr1.arpe = 1; /* auto-preload enable */
	tim3->cr1.dir = TIM_DIR_UPCNT; /* upcounter */
//	tim3->ccer.cc1p = 0; /* polarity */
//	tim3->ccer.cc1ne = 0;
//	tim3->egr.cc1g = 1; /* capture/compare generation */
	tim3->egr.ug = 1; /* update generation */
	tim3->bdtr.moe = 1; /* main output enable */
	tim3->cr1.cen = 1; /* enable */
}

void setup_tim14()
{
	if (tim14->cr1.cen)
		return;

	rcc->apb1enr.tim14en = 1;

	/* Setup tim14 */
	tim14->arr = 2550; /* auto-reload aka period */
	tim14->psc = 32000 - 1; /* prescaler */
	tim14->ccmr.ch_lcdbg.out.ccs = TIM_CCS_OUT; /* output */
	tim14->ccmr.ch_lcdbg.out.ocm = 6; /* 110 -- pwm mode 1 */
	tim14->ccmr.ch_lcdbg.out.ocpe = 1; /* preload enable, enables changing ccr1
					on the fly */

//	tim14->cr1.arpe = 1; /* auto-preload enable */
	tim14->cr1.dir = TIM_DIR_UPCNT; /* upcounter */
//	tim14->ccer.cc1p = 0; /* polarity */
//	tim14->ccer.cc1ne = 0;
//	tim14->egr.cc1g = 1; /* capture/compare generation */
	tim14->egr.ug = 1; /* update generation */
	tim14->bdtr.moe = 1; /* main output enable */
	tim14->cr1.cen = 1; /* enable */
}

#endif
