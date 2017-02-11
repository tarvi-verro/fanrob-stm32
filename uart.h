#pragma once


extern int uart_readbuf_length();
extern void uart_putc(char c);
extern void uart_puts(const char *s);
extern void uart_puts_visible(const char *s);
extern void uart_puts_hex(unsigned z, int minwidth);
extern void uart_puts_hex_0x(unsigned z);
extern void uart_puts_int(unsigned z) __attribute__ ((deprecated));
extern void uart_puts_unsigned(unsigned z);

extern void setup_uart();

#ifdef CFG_UART
struct uart_configuration {
	enum pin tx;
	enum pin rx;
	int alt;
	volatile struct lpuart_reg *lpuart;
	volatile struct dma_reg *dma;
	int dma_sel_lpuart_rx; // See dma->cselr.chX.cs
};
static const struct uart_configuration cfg_uart;
#endif
