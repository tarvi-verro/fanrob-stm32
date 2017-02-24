#include "rtc.h"

extern void clock_get(struct rtc_dr *date, struct rtc_tr *time,
		struct rtc_ssr *subsec);

extern void clock_set(struct rtc_dr date, struct rtc_tr time);

extern void setup_clock(void);

extern void clock_alarm(struct rtc_alrmar alrm, struct rtc_alrmassr ss,
		void (*cb)());

extern void clock_alarm_stop(void (*cb)());
extern void clock_cmd(char *cmd, int len);

/*
 * seconds since startup
 */
extern unsigned clock_seconds;
