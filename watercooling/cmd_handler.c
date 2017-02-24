
#include "cmd.h"
#include "clock.h"
#include "uart.h"
#include "fanctl.h"
#include "one-wire.h"
#include "dynamic.h"

void getclock();

void (*cmd_handle_override)(char *cmd, int len) = NULL;

void cmd_handle(char *cmd, int len)
{
	if (cmd_handle_override != NULL) {
		cmd_handle_override(cmd, len);
		return;
	}

	if (len < 1)
		return;

	switch (*cmd) {
	case 'c':
		clock_cmd(cmd, len);
		break;
	case 's':
		getclock(cmd, len);
		break;
	case 'd':
		dyn_cmd(cmd, len);
		break;
	case 'f':
	case 'R':
	case 'r':
	case 'v':
	case 'V':
	case 'w':
		fanctl_cmd(cmd, len);
		break;
	case 'o':
		ow_cmd(cmd, len);
		break;
	/*case 'p':
		pump_cmd(cmd, len);
		break;*/
	case '?':
		uart_puts("List of commands:\r\n"
				"\tc*: clock (get,set)\r\n"
				"\tcn: seconds since startup\r\n"
				"\tf: set fan speed cycle\r\n"
				"\tR: get RPM counter\r\n"
				"\tw: watch RPM counter\r\n"
				"\tr: set automatic rpm speed, show info\r\n"
				"\ts: display some info about frequencies\r\n"
				"\to: one-wire commands\r\n"
				"\td[w|i|s[?]|?]: dynamic commands\r\n"
				"\t?: display this\r\n");
		break;
	default:
		uart_puts("Unknown command. '?' for help.\r\n");
#if 1
		uart_puts("Full command: \"");
		uart_puts_visible(cmd);
		uart_puts("\"\r\n");
#endif
	}

}

