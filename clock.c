
#include <stdbool.h>
extern void assert(bool);

#include "conf.h"
#include "l4-rtc.h"
#include "l4-rcc.h"
#include "l4-pwr.h"
#include "l4-exti.h"
#include "clock.h"


void (*alarm_callb)() = NULL;

#if 0
void clock_exti_rtc()
{
	exti->pr.pr17 = 1; /* writing 1 clears bit */
	assert(rtc->isr.alraf);
	rtc->isr.alraf = 0;
	if (alarm_callb == NULL)
		return;
	alarm_callb();
}

void clock_alarm(struct rtc_alrmar alrm, struct rtc_alrmassr ss, void (*cb)())
{
	rtc->wpr.key = 0xca; /* unlock write protection */
	rtc->wpr.key = 0x53;

	rtc->cr.alrae = 0;
	while (!rtc->isr.alrawf);

	ss.maskss = 0xf;
	rtc->alrmar = alrm;
	rtc->alrmassr = ss;
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

#endif

void clock_set(struct rtc_dr date, struct rtc_tr time)
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

void clock_init_lse()
{
	rcc->bdcr.rtcsel = RCC_RTCSEL_LSE;

	rcc->bdcr.lseon = 1;
	rcc->bdcr.lsebyp = 0;
	while (!rcc->bdcr.lserdy); /* wait for lse to get ready */
}

void setup_clock(void)
{
	rcc->apb1enr1.pwren = 1;
	while (!rcc->apb1enr1.pwren);

	pwr->cr1.dbp = 1;

#ifdef LSI
	_Static_assert(1==2, "Not supported?");
	rcc->bdcr.rtcsel = RCC_RTCSEL_LSI;

	rcc->csr.lsion = 1;
	while (!rcc->csr.lsirdy); /* wait for lsi to get ready */
#else
	clock_init_lse();
#endif

	rcc->bdcr.rtcen = 1;

	/* Enable RTC interrupts
	 * "RTC Interrupts (combined EXTI lines 17, 19, and 20)" */
	//exti->imr.mr19 = exti->imr.mr20 = exti->imr.mr17 = 1;
	//exti->rtsr.tr19 = exti->rtsr.tr20 = exti->rtsr.tr17 = 1;
	//exti->pr.pr19 = exti->pr.pr20 = exti->pr.pr17 = 1;
	//nvic_iser[0] |= 1 << 2;

	if (rtc->isr.inits) /* already setup */
		return;
	/* ... RTC hasn't been run before */

	rtc->wpr.key = 0xca; /* unlock write protection */
	rtc->wpr.key = 0x53;

	rtc->isr.init = 1;
	while (!rtc->isr.initf); /* wait for init mode */

#ifdef LSI
	/* prediv lsi (~40kHz) to gen 1Hz */
	rtc->prer.prediv_s = 320 - 1;
	rtc->prer.prediv_a = 125 - 1;
#else
	/* prediv lse (32kHz) to gen 1Hz */
	rtc->prer.prediv_a = 128 - 1;
	rtc->prer.prediv_s = 256 - 1;
#endif

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

void clock_get(struct rtc_dr *date, struct rtc_tr *time,
		struct rtc_ssr *subsec)
{
	assert(rtc->isr.rsf);
	if (time != NULL)
		*time = rtc->tr;

	if (date != NULL)
		*date = rtc->dr;
//	*subsec = rtc->ssr;
}

