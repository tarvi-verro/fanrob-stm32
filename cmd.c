
#include "regs.h"
#include "cmd.h"
#include "uart.h"
#include "dma.h"
#include "fanctl.h"

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
		uart_puts("\r\nWhoa who, slow down there cowboy!\r\n");
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
			uart_puts("\r\n");
			cmd[cmd_len] = '\0';
			cmd_handle(cmd,cmd_len);
			cmd_len = 0;
			continue;
		} else if (b == 0x03) { /* Ctrl+C */
			uart_puts("^C\r\n");
			cmd_len = 0;
			continue;
		} else if (b == 0x7f && cmd_len) { /* Backspace */
			cmd_len--;
			uart_puts("\x08 \x08"); // Output ^H
			continue;
		} else if (b == 0x1b) {
			uart_puts("\\x1b");
			continue;
		}
		uart_putc(b);
		if (b != 0) {
			cmd[cmd_len] = b;
			cmd_len++;
		}

		if (cmd_len == sizeof(cmd) - 1) {
			uart_puts("\r\nToo long a command!\r\n");
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

extern void clock_cmd(char *cmd, int len);

 __attribute__ ((weak)) void cmd_handle(char *cmd, int len)
{
	uart_puts("Full command: \"");
	uart_puts_visible(cmd);
	uart_puts("\"\r\n");
}


