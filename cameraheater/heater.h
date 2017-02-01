#include <stdint.h>

extern void setup_heater();
extern void heater_set(uint8_t b);

#ifdef CFG_HEAT
struct heater_configuration {
	enum pin mosfet;
	int gpio_af_tim;
	enum tim_preset_ch mosfet_tim_slow_ch;

	enum pin battery;
};
static const struct heater_configuration cfg_heat;
#endif
