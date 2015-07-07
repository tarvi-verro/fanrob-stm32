
#include <stdbool.h>
extern void assert(bool);

#include "conf.h"
#include "f0-rtc.h"
#include "f0-rcc.h"
#include "f0-pwr.h"
#include "f0-exti.h"
#include "clock.h"


void (*alarm_callb)() = NULL;

void clock_exti_rtc()
{
	exti->pr.pr17 = 1; /* writing 1 clears bit */
	assert(rtc->isr.alraf);
	rtc->isr.alraf = 0;
	if (alarm_callb == NULL)
		return;
	alarm_callb();
}

void clock_alarm(struct rtc_alrmar alrm, void (*cb)())
{
	rtc->wpr.key = 0xca; /* unlock write protection */
	rtc->wpr.key = 0x53;

	rtc->cr.alrae = 0;
	while (!rtc->isr.alrawf);

	rtc->alrmar = alrm;
	rtc->cr.osel = RTC_OSEL_ALARM_A;
	rtc->cr.alraie = 1;
	rtc->cr.alrae = 1;

	rtc->wpr.key = 0x11; /* relock write protection */
	alarm_callb = cb;
}

void clock_alarm_stop(void (*cb)())
{
	assert(alarm_callb != NULL);
	assert(alarm_callb == cb);
	alarm_callb = NULL;

	rtc->wpr.key = 0xca; /* unlock write protection */
	rtc->wpr.key = 0x53;

	rtc->cr.osel = RTC_OSEL_DISABLED;
	rtc->cr.alrae = 0;
	rtc->cr.alraie = 0;

	rtc->wpr.key = 0x11; /* relock write protection */
}

void clock_set(struct rtc_tr time, struct rtc_dr date)
{
	rtc->wpr.key = 0xca; /* unlock write protection */
	rtc->wpr.key = 0x53;

	rtc->isr.init = 1;
	while (!rtc->isr.initf); /* wait for init mode */

	rtc->tr = time;
	rtc->dr = date;

	rtc->isr.init = 0;
	rtc->wpr.key = 0x11; /* relock write protection */
	while (!rtc->isr.rsf);
}

void setup_clock(void)
{
	rcc->apb1enr.pwr_en = 1;
	while(!rcc->apb1enr.pwr_en);

	pwr->cr.dbp = 1;

	rcc->bdcr.rtcsel = RCC_RTCSEL_LSI;

	rcc->csr.lsion = 1;
	while(!rcc->csr.lsirdy); /* wait for lsi to get ready */

	rcc->bdcr.rtc_en = 1;

	/* Enable RTC interrupts
	 * "RTC Interrupts (combined EXTI lines 17, 19, and 20)" */
	exti->imr.mr19 = exti->imr.mr20 = exti->imr.mr17 = 1;
	exti->rtsr.tr19 = exti->rtsr.tr20 = exti->rtsr.tr17 = 1;
	exti->pr.pr19 = exti->pr.pr20 = exti->pr.pr17 = 1;
	nvic_iser[0] |= 1 << 2;

	if (rtc->isr.inits) /* already setup */
		return;
	/* ... RTC hasn't been run before */

	rtc->wpr.key = 0xca; /* unlock write protection */
	rtc->wpr.key = 0x53;

	rtc->isr.init = 1;
	while (!rtc->isr.initf); /* wait for init mode */

	/* prediv lsi (~40kHz) to gen 1Hz */
	rtc->prer.prediv_s = 320;
	rtc->prer.prediv_a = 125 - 1;

	/* load initial date/time */
	struct rtc_tr a = {
		.su = 1,	.st = 3,
		.mnu = 7,	.mnt = 3,
		.hu = 2,	.ht = 0,
		.pm = RTC_PM_24H,
	};
	rtc->tr = a;
	struct rtc_dr b = {
		.du = 1,	.dt = 3,
		.mu = 5,	.mt = 0,
		.wdu = 7,
		.yu = 5,	.yt = 1,
	};
	rtc->dr = b;

	rtc->cr.fmt = RTC_FMT_24H;

	rtc->isr.init = 0;
	rtc->wpr.key = 0xff; /* random key write enables protection again */
	while (!rtc->isr.rsf);
}

void clock_get(struct rtc_tr *time, struct rtc_dr *date)
{
	assert(rtc->isr.rsf);
	*time = rtc->tr;
	*date = rtc->dr;
}

