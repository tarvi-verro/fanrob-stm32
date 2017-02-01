#pragma once

extern void setup_fanctl(); /* fanctl.c */
extern void fanctl_setspeed(uint8_t speed);
extern void fanctl_cmd(char *cmd, int len);

enum fanctl_tim_ch {
	TIM_CH1,
	TIM_CH2,
	TIM_CH3,
	TIM_CH4
};


#ifdef CFG_FANCTL
struct fanctl_configuration {
	enum pin rpm;
	enum pin ctl;
	int ctl_tim_af;
	enum fanctl_tim_ch ctl_tim_ch;
	volatile struct tim_reg *ctl_tim;
};
static const struct fanctl_configuration cfg_fan;
#endif
