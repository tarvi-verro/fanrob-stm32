#include "gpio-abs.h"
#include "tim.h"
#define CFG_FAST
#define CFG_SLOW
#include "tim-preset.h"
#include "conf.h"

#include "rcc-abs.h"
#include "assert-c.h"

void tim_slow_duty_set(enum tim_preset_ch ch, uint8_t d)
{
	volatile struct tim_reg *tim = cfg_slow.tim;
	switch (ch) {
	case TIM_CH1: tim->ccr1 = d * 10; break;
	case TIM_CH2: tim->ccr2 = d * 10; break;
	case TIM_CH3: tim->ccr3 = d * 10; break;
	case TIM_CH4: tim->ccr4 = d * 10; break;
	default: assert(0);
	}
}

uint8_t tim_fast_duty_get(enum tim_preset_ch ch)
{
	volatile struct tim_reg *tim = cfg_fast.tim;
	unsigned period = tim->arr;

	unsigned spd;
	switch (ch) {
	case TIM_CH1: spd = tim->ccr1; break;
	case TIM_CH2: spd = tim->ccr2; break;
	case TIM_CH3: spd = tim->ccr3; break;
	case TIM_CH4: spd = tim->ccr4; break;
	default: assert(0);
	}

	// [0,period-1] → [0,255]
	unsigned div = ((period-1) * 10000 / 255 + 5) / 10;
	return (uint8_t) ((spd * 10000 / div + 5) / 10);
}

void tim_fast_duty_set(enum tim_preset_ch ch, uint8_t duty)
{
	volatile struct tim_reg *tim = cfg_fast.tim;
	unsigned period = tim->arr;

	// [0,255] → [0,period-1]
	unsigned div = (255 * 10000 / (period-1) + 5) / 10;
	unsigned nv = (duty * 10000 / div + 5) / 10;

	switch (ch) {
	case TIM_CH1: tim->ccr1 = nv; break;
	case TIM_CH2: tim->ccr2 = nv; break;
	case TIM_CH3: tim->ccr3 = nv; break;
	case TIM_CH4: tim->ccr4 = nv; break;
	default: assert(0);
	}
}

void setup_tim_slow()
{
	volatile struct tim_reg *tim = cfg_slow.tim;
	if (tim->cr1.cen)
		return;

#ifdef CONF_F0
	switch ((intptr_t) tim) {
	case (intptr_t) tim3_macro:	rcc->apb1enr.tim3en = 1;	break;
	case (intptr_t) tim1_macro:	rcc->apb2enr.tim1en = 1;	break;
	case (intptr_t) tim14_macro:	rcc->apb1enr.tim14en = 1;	break;
	default:	assert(1==3);
	}
#elif defined(CONF_L4)
	switch ((intptr_t) tim) {
	case (intptr_t) tim2_macro:	rcc->apb1enr1.tim2en = 1;	break;
	case (intptr_t) tim1_macro:	rcc->apb2enr.tim1en = 1;	break;
	case (intptr_t) tim15_macro:	rcc->apb2enr.tim15en = 1;	break;
	case (intptr_t) tim16_macro:	rcc->apb2enr.tim16en = 1;	break;
	default:	assert(0);
	}
#endif

	/* Setup tim */
	tim->arr = 2550; /* auto-reload aka period */
#ifdef CONF_L0
	tim->psc = 32000 - 1; /* prescaler */
#elif defined (CONF_L4)
	tim->psc = 16000 - 1; /* prescaler */
#endif
	if (cfg_slow.cc1e)
		tim->ccmr.ch1 = cfg_slow.ch1;
	if (cfg_slow.cc2e)
		tim->ccmr.ch2 = cfg_slow.ch2;
	if (cfg_slow.cc3e)
		tim->ccmr.ch3 = cfg_slow.ch3;
	if (cfg_slow.cc4e)
		tim->ccmr.ch4 = cfg_slow.ch4;
//	tim->cr1.arpe = 1; /* auto-preload enable */
	tim->cr1.dir = TIM_DIR_UPCNT; /* upcounter */
//	tim->ccer.cc1p = 0; /* polarity */
//	tim->ccer.cc1ne = 0;
//	tim->egr.cc1g = 1; /* capture/compare generation */
	tim->egr.ug = 1; /* update generation */
#ifndef CONF_L0
	tim->bdtr.moe = 1; /* main output enable */
#endif
	tim->cr1.cen = 1; /* enable */

	if (cfg_slow.cc1e)
		tim->ccer.cc1e = 1;
	if (cfg_slow.cc2e)
		tim->ccer.cc2e = 1;
	if (cfg_slow.cc3e)
		tim->ccer.cc3e = 1;
	if (cfg_slow.cc4e)
		tim->ccer.cc4e = 1;
}

void setup_tim_fast()
{
	volatile struct tim_reg *tim = cfg_fast.tim;
	if (tim->cr1.cen)
		return;

#ifdef CONF_F0
	switch ((intptr_t) tim) {
	case (intptr_t) tim3_macro:	rcc->apb1enr.tim3en = 1;	break;
	case (intptr_t) tim1_macro:	rcc->apb2enr.tim1en = 1;	break;
	case (intptr_t) tim14_macro:	rcc->apb1enr.tim14en = 1;	break;
	default:	assert(1==3);
	}
#elif defined(CONF_L4)
	switch ((intptr_t) tim) {
	case (intptr_t) tim2_macro:	rcc->apb1enr1.tim2en = 1;	break;
	case (intptr_t) tim1_macro:	rcc->apb2enr.tim1en = 1;	break;
	case (intptr_t) tim15_macro:	rcc->apb2enr.tim15en = 1;	break;
	case (intptr_t) tim16_macro:	rcc->apb2enr.tim16en = 1;	break;
	default:	assert(0);
	}
#elif defined(CONF_L0)
	switch ((intptr_t) tim) {
	case (intptr_t) tim2_macro:	rcc->apb1enr.tim2en = 1;	break;
	case (intptr_t) tim3_macro:	rcc->apb1enr.tim3en = 1;	break;
	case (intptr_t) tim6_macro:	rcc->apb1enr.tim6en = 1;	break;
	case (intptr_t) tim7_macro:	rcc->apb1enr.tim7en = 1;	break;
	case (intptr_t) tim21_macro:	rcc->apb2enr.tim21en = 1;	break;
	case (intptr_t) tim22_macro:	rcc->apb2enr.tim22en = 1;	break;
	default:	assert(0);
	}
#endif

	/* Setup tim */
	unsigned period = (rcc_get_sysclk()*10/cfg_fast.frequency + 5) / 10;
	assert(period < UINT16_MAX && period >= 20);
	tim->arr = period; /* auto-reload aka period */
	tim->psc = 0; /* prescaler */
	if (cfg_fast.cc1e)
		tim->ccmr.ch1 = cfg_fast.ch1;
	if (cfg_fast.cc2e)
		tim->ccmr.ch2 = cfg_fast.ch2;
	if (cfg_fast.cc3e)
		tim->ccmr.ch3 = cfg_fast.ch3;
	if (cfg_fast.cc4e)
		tim->ccmr.ch4 = cfg_fast.ch4;
//	tim->cr1.arpe = 1; /* auto-preload enable */
	tim->cr1.dir = TIM_DIR_UPCNT; /* upcounter */
//	tim->ccer.cc1p = 0; /* polarity */
//	tim->ccer.cc1ne = 0;
//	tim->egr.cc1g = 1; /* capture/compare generation */
	tim->egr.ug = 1; /* update generation */
#ifndef CONF_L0
	tim->bdtr.moe = 1; /* main output enable */
#endif
	tim->cr1.cen = 1; /* enable */

	if (cfg_fast.cc1e)
		tim->ccer.cc1e = 1;
	if (cfg_fast.cc2e)
		tim->ccer.cc2e = 1;
	if (cfg_fast.cc3e)
		tim->ccer.cc3e = 1;
	if (cfg_fast.cc4e)
		tim->ccer.cc4e = 1;
}

uint8_t tim_fast_count()
{
	volatile struct tim_reg *tim = cfg_fast.tim;
	unsigned period = tim->arr;
	// cnt is in range [0..period-1]
	unsigned cnt = tim->cnt;
	cnt = cnt * UINT8_MAX / period;
	return cnt;
}

