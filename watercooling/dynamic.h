#include <stdint.h>

extern void dyn_cmd(char *cmd, int len);
extern void dyn_mainloop();
extern void setup_dynamic();

#ifdef CFG_DYNAMIC
struct dynamic_configuration {
	uint8_t temperature[8];
	int fan_mobo;
	int fan_water_top;
	int fan_water_bot;

	unsigned water_rpm_max;
	unsigned water_rpm_rest;
	unsigned mobo_rpm_max;
	unsigned mobo_rpm_rest;

	unsigned temp_high;
	unsigned temp_low;
};

static struct dynamic_configuration cfg_dyn;
#endif
