
#include "cmd.h"
#include "clock.h"
#include "uart.h"
#include "fanctl.h"

void cmd_handle(char *cmd, int len)
{
	if (len < 1)
		return;

	switch (*cmd) {
	case 'c':
		clock_cmd(cmd, len);
		break;
	case 'f':
	case 'R':
	case 'r':
		fanctl_cmd(cmd, len);
		break;
	case '?':
		uart_puts("List of commands:\r\n"
				"\tc*: clock (get,set)\r\n"
				"\tcn: seconds since startup\r\n"
				"\tf: set fan speed cycle\r\n"
				"\tR: get RPM counter\r\n"
				"\tr: set target RPM\r\n"
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

