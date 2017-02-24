#pragma once

extern void setup_fanctl(); /* fanctl.c */
extern void fanctl_settarget_dyn(int fan, unsigned rpm); /* doesn't switch strategy */
extern unsigned fanctl_gettarget_dyn(int fan);
extern int fanctl_getspeed(int fan);
extern void fanctl_cmd(char *cmd, int len);
extern void fanctl_rpm_measure(int cnt); /* should be called from main loop */


#ifdef CFG_FANCTL
struct fanctl_configuration {
	enum pin pwr_in;
	unsigned step;
	struct fan_configuration {
		enum pin rpm;
		enum pin ctl;
		int ctl_tim_af;
		enum tim_preset_ch ctl_fast_ch;
		uint8_t ctl_initial_duty;
	} fans[4];
};
static const struct fanctl_configuration cfg_fan;
#endif
