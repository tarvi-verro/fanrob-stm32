#define CFG_DYNAMIC
#include "dynamic.h"
#include "conf.h"
#include "uart.h"
#include "fanctl.h"
#include "clock.h"
#include "one-wire.h"
#include <string.h>
#include "decimal.h"

static void cmd_handle_override_dyn(char *cmd, int len);
extern void (*cmd_handle_override)(char *cmd, int len); /* cmd_handler.c */

static int print_thing = 0;
static int retarget = 0;

void dyn_1hz()
{
	if (clock_seconds & 1)
		return;
	print_thing = cmd_handle_override == cmd_handle_override_dyn;

	if ((clock_seconds & 0x7) != 0x0)
		return;
	retarget = 1;
}

void dyn_mainloop()
{
	if (!print_thing && !retarget)
		return;

	int rpm_mobo = fanctl_getspeed(cfg_dyn.fan_mobo);
	int rpm_wtop = fanctl_getspeed(cfg_dyn.fan_water_top);
	int rpm_wbot = fanctl_getspeed(cfg_dyn.fan_water_bot);
	int temp = ow_measure_temp(cfg_dyn.temperature);

	if (print_thing) {
		uart_puts("T (°mC): ");
		uart_puts_unsigned(temp);

		uart_puts(",\tM: ");
		uart_puts_unsigned(rpm_mobo);

		uart_puts(", WT: ");
		uart_puts_unsigned(rpm_wtop);

		uart_puts(", WB: ");
		uart_puts_unsigned(rpm_wbot);

		uart_puts("\r\n");
		print_thing = 0;
	}

	if (!retarget)
		return;
	retarget = 0;

	if (cfg_dyn.temp_low >= temp) {
		fanctl_settarget_dyn(cfg_dyn.fan_mobo, cfg_dyn.mobo_rpm_rest);
		fanctl_settarget_dyn(cfg_dyn.fan_water_top, cfg_dyn.water_rpm_rest);
		fanctl_settarget_dyn(cfg_dyn.fan_water_bot, cfg_dyn.water_rpm_rest);
	} else if (cfg_dyn.temp_high > temp) {
		// Mobo fan
		int temp_d = cfg_dyn.temp_high - cfg_dyn.temp_low;
		int mobo_rpm_d = cfg_dyn.mobo_rpm_max - cfg_dyn.mobo_rpm_rest;
		int m_nxt = (temp - cfg_dyn.temp_low) * 1000 / temp_d * mobo_rpm_d / 1000;
		fanctl_settarget_dyn(cfg_dyn.fan_mobo, m_nxt + cfg_dyn.mobo_rpm_rest);

		// Water cooling fan
		int water_rpm_d = cfg_dyn.water_rpm_max - cfg_dyn.water_rpm_rest;
		int w_nxt = (temp - cfg_dyn.temp_low) * 1000 / temp_d * water_rpm_d / 1000;
		fanctl_settarget_dyn(cfg_dyn.fan_water_top,
				w_nxt + cfg_dyn.water_rpm_rest);
		fanctl_settarget_dyn(cfg_dyn.fan_water_bot,
				w_nxt + cfg_dyn.water_rpm_rest);
	} else {
		fanctl_settarget_dyn(cfg_dyn.fan_mobo, cfg_dyn.mobo_rpm_max);
		fanctl_settarget_dyn(cfg_dyn.fan_water_top, cfg_dyn.water_rpm_max);
		fanctl_settarget_dyn(cfg_dyn.fan_water_bot, cfg_dyn.water_rpm_max);
	}
}

static void cmd_handle_override_dyn(char *cmd, int len)
{
	cmd_handle_override = NULL;
	print_thing = 0;
}

void dyn_cmd(char *cmd, int len)
{
	if (len == 2 && cmd[1] == '?') {
		uart_puts("List of commands for dynamic:\r\n"
				"\tdw: watch temp/speeds\r\n"
				"\tdi: show dynamic settings\r\n"
				"\tds: set dynamic settings\r\n"
			 );
	} else if (len == 2 && cmd[1] == 'i') {
		uart_puts("Watercooling fan speed: [");
		uart_puts_unsigned(cfg_dyn.water_rpm_rest);
		uart_putc(':');
		uart_puts_unsigned(cfg_dyn.water_rpm_max);
		uart_puts("]\r\n");

		uart_puts("Motherboard (cpu area) fan speed: [");
		uart_puts_unsigned(cfg_dyn.mobo_rpm_rest);
		uart_putc(':');
		uart_puts_unsigned(cfg_dyn.mobo_rpm_max);
		uart_puts("]\r\n");

		uart_puts("Temperature: [");
		uart_puts_unsigned(cfg_dyn.temp_low);
		uart_putc(':');
		uart_puts_unsigned(cfg_dyn.temp_high);
		uart_puts("]\r\n");
	} else if (len >= 2 && cmd[1] == 's') {
		if (len >= 14 // ds max water 900
				&& !memcmp(cmd+3, "max ", 4)
				&& !memcmp(cmd+7, "water ", 6)) {
			int ns = parseBase10(cmd + 13, len - 13);
			cfg_dyn.water_rpm_max = ns;
			uart_puts("Set max water rpm to ");
			uart_puts_unsigned(cfg_dyn.water_rpm_max);
			uart_puts(".\r\n");
		} else if (len >= 15 // ds rest water 500
				&& !memcmp(cmd+3, "rest ", 5)
				&& !memcmp(cmd+8, "water ", 6)) {
			int ns = parseBase10(cmd + 14, len - 14);
			cfg_dyn.water_rpm_rest = ns;
			uart_puts("Set rest water rpm to ");
			uart_puts_unsigned(cfg_dyn.water_rpm_rest);
			uart_puts(".\r\n");
		} else if (len >= 13 // ds max mobo 1800
				&& !memcmp(cmd+3, "max ", 4)
				&& !memcmp(cmd+7, "mobo ", 5)) {
			int ns = parseBase10(cmd + 12, len - 12);
			cfg_dyn.mobo_rpm_max = ns;
			uart_puts("Set max mobo rpm to ");
			uart_puts_unsigned(cfg_dyn.mobo_rpm_max);
			uart_puts(".\r\n");
		} else if (len >= 14 // ds rest mobo 1800
				&& !memcmp(cmd+3, "rest ", 5)
				&& !memcmp(cmd+8, "mobo ", 5)) {
			int ns = parseBase10(cmd + 13, len - 13);
			cfg_dyn.mobo_rpm_rest = ns;
			uart_puts("Set rest mobo rpm to ");
			uart_puts_unsigned(cfg_dyn.mobo_rpm_rest);
			uart_puts(".\r\n");
		} else if (len >= 9 // ds high 41000
				&& !memcmp(cmd+3, "high ", 5)) {
			int ns = parseBase10(cmd + 8, len - 8);
			cfg_dyn.temp_high = ns;
			uart_puts("Set high temperature to ");
			uart_puts_unsigned(cfg_dyn.temp_high);
			uart_puts("°mC.\r\n");
		} else if (len >= 8 // ds low 35000
				&& !memcmp(cmd+3, "low ", 4)) {
			int ns = parseBase10(cmd + 7, len - 7);
			cfg_dyn.temp_low = ns;
			uart_puts("Set low temperature to ");
			uart_puts_unsigned(cfg_dyn.temp_low);
			uart_puts("°mC.\r\n");
		} else {
			uart_puts("Usage: ds [max|rest] [mobo|water] rpm\r\n");
			uart_puts("       ds [high|low] temp(°mC)\r\n");
		}
	} else if (len == 2 && cmd[1] == 'w') {
		cmd_handle_override = cmd_handle_override_dyn;
	} else {
		uart_puts("Invalid command, see 'd?'.\r\n");
	}
}

void setup_dynamic()
{
	fanctl_settarget_dyn(cfg_dyn.fan_water_top, cfg_dyn.water_rpm_rest);
	fanctl_settarget_dyn(cfg_dyn.fan_water_bot, cfg_dyn.water_rpm_rest);
	fanctl_settarget_dyn(cfg_dyn.fan_mobo, cfg_dyn.mobo_rpm_rest);
}

