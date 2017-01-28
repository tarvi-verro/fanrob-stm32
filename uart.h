#pragma once


extern int uart_readbuf_length();
extern void uart_putc(char c);
extern void uart_puts(const char *s);
extern void uart_puts_visible(const char *s);
extern void uart_puts_int(unsigned z);

extern void setup_uart();

