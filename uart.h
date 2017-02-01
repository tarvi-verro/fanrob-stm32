#pragma once


extern int uart_readbuf_length();
extern void uart_putc(char c);
extern void uart_puts(const char *s);
extern void uart_puts_visible(const char *s);
extern void uart_puts_int(unsigned z);

extern void setup_uart();

#ifdef CFG_UART
struct uart_configuration {
	enum pin tx;
	enum pin rx;
	int alt;
};
static const struct uart_configuration cfg_uart;
#endif
