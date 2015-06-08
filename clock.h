#include "f0-rtc.h"

extern void clock_get(struct rtc_tr *time, struct rtc_dr *date);
extern void clock_set(struct rtc_tr time, struct rtc_dr date);
extern void setup_clock(void);

extern void clock_alarm(struct rtc_alrmar alrm, void (*cb)());
extern void clock_alarm_stop(void (*cb)());

