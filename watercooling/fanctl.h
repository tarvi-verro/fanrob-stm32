#pragma once

extern void setup_fanctl(); /* fanctl.c */
extern void fanctl_setspeed(uint8_t speed);
extern void fanctl_cmd(char *cmd, int len);


#ifdef CFG_FANCTL
struct fanctl_configuration {
	enum pin rpm;
	enum pin ctl;
	int ctl_tim_af;
	enum tim_preset_ch ctl_fast_ch;
	uint8_t ctl_initial_duty;
	enum pin pwr_in;
};
static const struct fanctl_configuration cfg_fan;
#endif
