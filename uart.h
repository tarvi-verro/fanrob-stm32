#pragma once


extern int uart_readbuf_length();
extern void uart_send_byte(uint8_t byte);
extern void uart_print(const char *s);
void uart_print_int(unsigned z);

extern void setup_uart();

