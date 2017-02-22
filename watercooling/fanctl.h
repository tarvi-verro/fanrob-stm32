#pragma once

extern void setup_fanctl(); /* fanctl.c */
extern void fanctl_setspeed(int fan, uint8_t speed);
extern void fanctl_cmd(char *cmd, int len);
extern void fanctl_rpm_measure(int cnt); /* should be called from main loop */


#ifdef CFG_FANCTL
struct fanctl_configuration {
	enum pin pwr_in;
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
