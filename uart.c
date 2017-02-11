#define CFG_UART
#include "gpio-abs.h"
#define CFG_ASSERT
#include "assert-c.h"
#include "uart.h"
#include "conf.h"

#ifdef CONF_F0
#warning uart not supported on target
int uart_readbuf_length() { return 0; }
void uart_puts_visible(const char *s) {}
void uart_putc(char c) {}
void uart_puts(const char *s) {}
void uart_puts_int(unsigned z) {}
void uart_puts_unsigned(unsigned z) {}

void setup_uart() {}
#else

#ifndef ic_dma_receiver
#error Configuration had to define ic_dma_receiver!
#endif
#ifndef dma_ch
#error Configuration had to define dma_ch!
#endif

#include "lpuart.h"
#include "exti.h"
#include "rcc.h"
#include "gpio.h"
#include "dma.h"
#include <stdbool.h>
#include "decimal.h"

#define _MAC2STR(X) #X
#define MAC2STR(X) _MAC2STR(X)

static void setup_uart_pins()
{
	struct gpio_conf gcfg = {
		.mode = GPIO_MODER_AF,
		.ospeed = GPIO_OSPEEDR_MEDIUM,
		.otype = GPIO_OTYPER_PP,
		.pupd = GPIO_PUPDR_NONE,
		.alt = cfg_uart.alt,
	};
	gpio_configure(cfg_uart.tx, &gcfg);

	gcfg.otype = GPIO_OTYPER_OD;
	gpio_configure(cfg_uart.rx, &gcfg);
}

void uart_puts_visible(const char *s)
{
	for (; *s != '\0'; s++) {
		char z = *s;
		if ((z >= 'A' && z <= 'Z') || (z >= 'a' && z <= 'z') || (z >= '0' && z <= '9')
				|| z == ' ' || z == '!' || z == '@' || z == '$' || z == '?' || z == '[' || z == ']') {
			uart_putc(*s);
			continue;
		}
		uart_putc('\\');
		uart_putc('x');
		char h = (z & 0xf0) >> 4;
		char l = z & 0xf;
		if (h > 9)	uart_putc('a' + (h % 10));
		else		uart_putc('0' + h);
		if (l > 9)	uart_putc('a' + (l % 10));
		else		uart_putc('0' + l);
	}
}

void uart_puts_hex_0x(unsigned z)
{
	if (!z) {
		uart_puts("0x0");
		return;
	}
	uart_puts("0x");
	int i;
	for (i = 0; !(z & 0xf0000000); z <<= 4, i++);
	for (; i < 8; z <<= 4, i++) {
		char h = (z >> 28) & 0xf;
		if (h > 9)
			uart_putc('A' + h - 10);
		else
			uart_putc('0' + h);
	}
}

void uart_puts_hex(unsigned z, int minwidth)
{
	if (!z) {
		return;
	}
	int i;
	for (i = 0; !(z & 0xf0000000) && i < 8 - minwidth; z <<= 4, i++);
	for (; i < 8; z <<= 4, i++) {
		char h = (z >> 28) & 0xf;
		if (h > 9)
			uart_putc('A' + h - 10);
		else
			uart_putc('0' + h);
	}
}

void uart_puts_int(unsigned z)
{
	char buf[sizeof("4294967295")];
	int j = 0;
	for (unsigned i = 1; i <= 1000000000; i*=10) {
		buf[j] = (z / i) % 10;
		j++;
		if (i == 1000000000)
			break;
	}
	for (int i = j - 1; i >= 0; i--)
		uart_putc('0' + buf[i]);
}

void uart_puts_unsigned(unsigned z)
{
	print_decimal(uart_putc, z, decimal_length(z));
}

void uart_puts(const char *s)
{
	for (; *s != '\0'; s++)
		uart_putc(*s);
}

void uart_putc(char c)
{
	volatile struct lpuart_reg *lpuart = cfg_uart.lpuart;
	//while(!lpuart->isr.idle);
	//while (!lpuart->isr.fe);
	//while (!lpuart->isr.pe);
	//while (!lpuart->isr.nf);

//	lpuart->cr1.te = 1;
	lpuart->cr1.te = 1;
	//while (!lpuart->isr.teack);
	while (!lpuart->isr.txe);
	lpuart->tdr.tdr = c;
	while (!lpuart->isr.txe);
	lpuart->cr1.te = 0;
	while (!lpuart->isr.tc);
}

extern void clock_init_lse(); // clock.c


char uart_readbuf[14] = { 0 };

void setup_uart()
{
	volatile struct lpuart_reg *lpuart = cfg_uart.lpuart;
	volatile struct dma_reg *dma = cfg_uart.dma;
	for (int i = 0; i < sizeof(uart_readbuf); i++)
		uart_readbuf[i] = 0;

	if (lpuart == lpuart1)
#ifdef CONF_L4
		rcc->apb1enr2.lpuart1en = 1;
#elif defined(CONF_L0)
		rcc->apb1enr.lpuart1en = 1;
#endif
	else
		assert(0);
	setup_uart_pins();

	int i;
	for (i = 0; i < 2000; i++)
		asm("nop");


	clock_init_lse();
	if (lpuart == lpuart1)
		rcc->ccipr.lpuart1sel = RCC_CLKSRC_LSE;
	else
		assert(0);

	struct lpuart_cr1 cr1 = {
		.ue = 0, // LPUART enable
		.uesm = 0, // enable in stop mode
		.re = 0, // receiver enable
		//.te = 1, // transmitter enable
		.te = 0, // transmitter enable
		.idleie = 0, // idle interrupt enable
		.rxneie = 0, // rxne interrupt enable
		.tcie = 0, // transmission complete interrupt enable
		.txeie = 0, // interrupt enable
		.peie = 0, // PE interrupt enable
		.ps = LPUART_PARITY_EVEN, // parity selection
		.pce = 0, // parity control enable
		.wake = LPUART_WAKE_IDLE, // receiver wakeup method
		.mme = 0, // mute mode enable
		.cmie = 0, // character match interrupt enable
		.dedt = 0, // driver enable de-assertion time TODO: might be a prob.
		.deat = 0, // driver enable assertion time TODO: might be prob.
		.m0 = 0, // word length, 00 → data in 8 bits
		.m1 = 0, // second bit, located a world apart in the register
	};


	lpuart->cr1 = cr1;

	lpuart->brr.brr = (256*32768/(9600/10) + 5)/10; // "LPUARTDIV"

	struct lpuart_cr2 cr2 = {
		.addm7 = 0, // 8-bit address detection/4-bit address detection
		.stop = 0, // 00 → 1 stop bit, 10 → 2 stop bits
		.swap = 0, // swap tx/rx pins
		.rxinv = 0, // rx pin active level inversion
		.txinv = 0, // tx pin active level inversion
		.datainv = 0, // binary data inversion
		.msbfirst = 0, // most significant bit first
		.add_0 = 0, // address of the lpuart node
		.add_1 = 0, // also address of the lpuart node
	};
	lpuart->cr2 = cr2;

	struct lpuart_cr3 cr3 = {
		.eie = 0, // error interrupt enable
		.hdsel = 0, // half-duplex enable
		.dmar = 1, // dma enable receiver
		.dmat = 0, // dma enable transmitter
		.rtse = 0, // RTS enable "hardware flow control"
		.ctse = 0, // CTS enable "hardware flow control"
		.ctsie = 0, // CTS interrupt enable
		.ovrdis = 1, // overrun disable (1 → disabled, 0 → enabled)
		.ddre = 0, // DMA disable on reception error
		.dem = 0, // driver enable mode
		.dep = 0, // driver enable polarity selection
		.wus = 0, // wakeup from stop mode interrupt flag selection
		.wufie = 0, // wakeup from stop mode interrupt enable
	};
	lpuart->cr3 = cr3;
	//lpuart->cr3.ovrdis = 1;

	lpuart->cr1.ue = 1;
//	lpuart->cr1.te = 0;
	while (!lpuart->cr1.ue);

#ifdef CONF_L4
	// Set up break on '\n' to parse full lines
	lpuart->cr2.add_0 = '\r' & 0x0f;
	lpuart->cr2.add_1 = ('\r' & 0xf0) >> 4;
	lpuart->cr1.cmie = 1; // enable character match interrupt
	nvic_iser[2] |= 1 << 6;
#elif defined(CONF_L0)
	/*
	 * For some reason on L0 setting cmie enables tcie. Also the character
	 * match events are never generated. Perhaps something to do with
	 * "through EXTI 28"?
	 *
	//lpuart1->cr1.tcie = 0;
	nvic_iser[0] |= 1 << 29;
	exti->imr.im28 = 1;
	exti->emr.em28 = 1;
	 */

#endif


	// Set up DMA
#ifdef CONF_L4
	if (dma == dma2)
		rcc->ahb1enr.dma2en = 1;
	else
		assert(0);
#elif defined(CONF_L0)
	if (dma == dma1)
		rcc->ahbenr.dma1en = 1;
	else assert(0);
#endif

	dma->dma_ch.cpar = (void *) &lpuart->rdr; // Peripheral address
	dma->dma_ch.cmar = uart_readbuf; // Memory address to push data to
	dma->dma_ch.cndtr.ndt = sizeof(uart_readbuf) - 1; // Total number of data to be transferred

	for (int z = 0; z < 0x8000; z++) asm("nop");

	struct dma_ccr ccr  = {
		.pl = DMA_PL_MEDIUM,
		.dir = DMA_DIR_FROM_PERIP,
		.circ = 1,
		.psize = DMA_PSIZE_8,
		.msize = DMA_MSIZE_8,
		.minc = 1, // Memory increment mode
		.pinc = 0, // Peripheral increment mode
		.tcie = 1, // Transfer complete interrupt
	};
	dma->dma_ch.ccr = ccr;
#ifdef CONF_L4
	if (dma == dma2 && (MAC2STR(dma_ch)[2] == '7'))
		nvic_iser[2] |= 1 << 5;
	else
		assert(0);
#elif defined(CONF_L0)
	if (dma == dma1 && (MAC2STR(dma_ch))[2] == '3')
		nvic_iser[0] |= 1 << 10;
	else
		assert(0);
#endif
	dma->cselr.dma_ch.cs = cfg_uart.dma_sel_lpuart_rx;

	dma->dma_ch.ccr.en = 1;

	lpuart->cr1.re = 1;
	while (!lpuart->isr.reack);
}

extern void cmd_rotate(); // cmd.c

void ic_dma_receiver()
{
	volatile struct dma_reg *dma = cfg_uart.dma;
	dma->ifcr.dma_ch.ctcif = 1;
	cmd_rotate();
}

int uart_readbuf_length()
{
	volatile struct dma_reg *dma = cfg_uart.dma;
	return sizeof(uart_readbuf) - dma->dma_ch.cndtr.ndt;
}

#ifdef CONF_L4
void i_lpuart1()
{
	volatile struct lpuart_reg *lpuart = cfg_uart.lpuart;
	// lpuart->isr.cmf		// character match flag
	// lpuart->icr.cmcf = 1	// reset character match flag
	assert(lpuart->isr.cmf); // character match flag
	lpuart->icr.cmcf = 1; // reset character match flag
	gpio_flip(cfg_assert.led);
}
#endif

#endif
