#include <stdint.h>

extern void setup_tim_fast();
extern void setup_tim_slow();

enum tim_preset_ch {
	TIM_CH1,
	TIM_CH2,
	TIM_CH3,
	TIM_CH4
};

extern uint8_t tim_fast_count();
extern void tim_fast_duty_set(enum tim_preset_ch ch, uint8_t d);
extern void tim_slow_duty_set(enum tim_preset_ch ch, uint8_t d);

#ifdef CFG_FAST
struct tim_fast_configuration {
	volatile struct tim_reg *tim;
	union tim_ccmr_ch ch1;
	union tim_ccmr_ch ch2;
	union tim_ccmr_ch ch3;
	union tim_ccmr_ch ch4;
	int cc1e : 1, cc2e : 1, cc3e : 1, cc4e : 1;
	unsigned frequency;
};
static const struct tim_fast_configuration cfg_fast;
#endif

#ifdef CFG_SLOW
struct tim_slow_configuration {
	volatile struct tim_reg *tim;
	union tim_ccmr_ch ch1;
	union tim_ccmr_ch ch2;
	union tim_ccmr_ch ch3;
	union tim_ccmr_ch ch4;
	int cc1e : 1, cc2e : 1, cc3e : 1, cc4e : 1;
};
static const struct tim_slow_configuration cfg_slow;
#endif
