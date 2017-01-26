
#include "conf.h"
#include "uart.h"
#include "dma.h"

static void cmd_handle();

extern char uart_readbuf[14]; /* uart.c */

static char cmd[15];
static int cmd_len = 0;

static int parsedTo = 0;

static int cmd_check_wlen(int len);
static int lostsome = 0;
static int rotatedAt = -1;
void cmd_check()
{
	int r = 1;
	if (rotatedAt != -1) {
		r = cmd_check_wlen(sizeof(uart_readbuf));
		parsedTo = 0;
		rotatedAt = -1;
	}
	if (r)
		cmd_check_wlen(uart_readbuf_length());
	else
		rotatedAt = uart_readbuf_length();
}

static int eat_next = 0;

static int cmd_check_wlen(int len)
{
	if (lostsome) {
		uart_print("\r\nWhoa who, slow down there cowboy!\r\n");
		lostsome = 0;
		parsedTo = len;
		return 0;
	}
	if (parsedTo >= len)
		return 1;

	for (; parsedTo < len; parsedTo++) {
		char b = uart_readbuf[parsedTo-1];
		if (eat_next) {
			eat_next--;
			continue;
		} else if (b == '\r') {
			uart_print("\r\n");
			cmd_handle(cmd,cmd_len);
			cmd_len = 0;
			continue;
		} else if (b == 0x03) { /* Ctrl+C */
			uart_print("^C\r\n");
			cmd_len = 0;
			continue;
		} else if (b == 0x7f && cmd_len) { /* Backspace */
			cmd_len--;
			uart_print("\x08 \x08"); // Output ^H
			continue;
		} else if (b == 0x1b) {
			uart_print("\\x1b");
			continue;
		}
		uart_send_byte(b);
		if (b != 0) {
			cmd[cmd_len] = b;
			cmd_len++;
		}

		if (cmd_len == sizeof(cmd) - 1) {
			uart_print("\r\nToo long a command!\r\n");
			goto abort;
		}
	}
	return 1;
abort:
	parsedTo = len;
	cmd_len = 0;
	return 0;
}

void cmd_rotate()
{
	if (rotatedAt != -1)
		lostsome++;
	rotatedAt = uart_readbuf_length();
}

extern void fanctl_cmd(char *cmd, int len);
extern void clock_cmd(char *cmd, int len);

extern void uart_print_visible(char *s);
static void cmd_handle(char *cmd, int len)
{
	if (len < 1)
		return;

	cmd[len] = '\0';
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
		uart_print("List of commands:\r\n"
				"\tc*: clock (get,set)\r\n"
				"\tcn: seconds since startup\r\n"
				"\tf: set fan speed cycle\r\n"
				"\tR: get RPM counter\r\n"
				"\tr: set target RPM"
				"\t?: display this\r\n");
		break;
	default:
		uart_print("Unknown command. '?' for help.\r\n");
#if 1
		uart_print("Full command: \"");
		uart_print_visible(cmd);
		uart_print("\"\r\n");
#endif

	}

}
