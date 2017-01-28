#include "conf.h"

#ifdef CONF_F0
#warning uart not supported on target
int uart_readbuf_length() { return 0; }
void uart_puts_visible(const char *s) {}
void uart_putc(char c) {}
void uart_puts(const char *s) {}
void uart_puts_int(unsigned z) {}

void setup_uart() {}
#else


#include "lpuart.h"
#include "exti.h"
#include "rcc.h"
#include "gpio.h"
#include "dma.h"
#include "uart.h"
#include <stdbool.h>


static void setup_uart_pins()
{
	rcc->iop_uart_rcc.iop_uart_en = 1;

	io_uart->moder.pin_uart_tx = GPIO_MODER_AF;
	io_uart->moder.pin_uart_rx = GPIO_MODER_AF;

#ifdef CONF_L4
	io_uart->afr.pin_uart_tx = 8;
	io_uart->afr.pin_uart_rx = 8;
#elif defined(CONF_L0)
	io_uart->afr.pin_uart_tx = 6;
	io_uart->afr.pin_uart_rx = 6;
#endif

	io_uart->ospeedr.pin_uart_tx = GPIO_OSPEEDR_MEDIUM;
	io_uart->ospeedr.pin_uart_rx = GPIO_OSPEEDR_MEDIUM;

	io_uart->otyper.pin_uart_tx = GPIO_OTYPER_PP;
	io_uart->otyper.pin_uart_rx = GPIO_OTYPER_OD;

	io_uart->pupdr.pin_uart_tx = GPIO_PUPDR_NONE;
	io_uart->pupdr.pin_uart_rx = GPIO_PUPDR_NONE;

	//io_uart->bsrr.reset.pin_uart_tx = 1;
	//io_uart->bsrr.reset.pin_uart_rx = 1;


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

void uart_puts(const char *s)
{
	for (; *s != '\0'; s++)
		uart_putc(*s);
}

void uart_putc(char c)
{
	//while(!lpuart1->isr.idle);
	//while (!lpuart1->isr.fe);
	//while (!lpuart1->isr.pe);
	//while (!lpuart1->isr.nf);

//	lpuart1->cr1.te = 1;
	lpuart1->cr1.te = 1;
	//while (!lpuart1->isr.teack);
	while (!lpuart1->isr.txe);
	lpuart1->tdr.tdr = c;
	while (!lpuart1->isr.txe);
	lpuart1->cr1.te = 0;
	while (!lpuart1->isr.tc);
}

extern void clock_init_lse(); // clock.c


void interr_dma2_ch7();

char uart_readbuf[14] = { 0 };

void setup_uart()
{
	for (int i = 0; i < sizeof(uart_readbuf); i++)
		uart_readbuf[i] = 0;

#ifdef CONF_L4
	rcc->apb1enr2.lpuart1en = 1;
#elif defined(CONF_L0)
	rcc->apb1enr.lpuart1en = 1;
#endif
	setup_uart_pins();

	int i;
	for (i = 0; i < 2000; i++)
		asm("nop");


	clock_init_lse();
	rcc->ccipr.lpuart1sel = RCC_CLKSRC_LSE;

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


	lpuart1->cr1 = cr1;

	lpuart1->brr.brr = 0x36a; // "LPUARTDIV"
	//lpuart1->brr.brr = 0xfa0ab; // "LPUARTDIV"

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
	lpuart1->cr2 = cr2;

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
	lpuart1->cr3 = cr3;
	//lpuart1->cr3.ovrdis = 1;

	lpuart1->cr1.ue = 1;
//	lpuart1->cr1.te = 0;
	while (!lpuart1->cr1.ue);

#ifdef CONF_L4
	// Set up break on '\n' to parse full lines
	lpuart1->cr2.add_0 = '\r' & 0x0f;
	lpuart1->cr2.add_1 = ('\r' & 0xf0) >> 4;
	lpuart1->cr1.cmie = 1; // enable character match interrupt
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
	/*
	 * For L4 DMA2: (hw registers page 302)
	 *	   Request number | Channel 6	| Channel 7
	 *			4 | LPUART_TX	| LPUART_RX
	 */
	rcc->ahb1enr.dma2en = 1;

	dma2->ch7.cpar = &lpuart1->rdr; // Peripheral address
	dma2->ch7.cmar = uart_readbuf; // Memory address to push data to
	dma2->ch7.cndtr.ndt = sizeof(uart_readbuf) - 1; // Total number of data to be transferred
#elif defined(CONF_L0)
	/*
	 * For L0 DMA1 (hw registers page 245, 253-254)
	 *	   Request number | Channel 2	| Channel 3
	 *	   		5 | LPUART1_TX	| LPUART1_RX
	 */
	rcc->ahbenr.dma1en = 1;

	dma1->ch3.cpar = &lpuart1->rdr; // Peripheral address
	dma1->ch3.cmar = uart_readbuf; // Memory address to push data to
	dma1->ch3.cndtr.ndt = sizeof(uart_readbuf) - 1; // Total number of data to be transferred
#endif

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
#ifdef CONF_L4
	dma2->ch7.ccr = ccr;
	nvic_iser[2] |= 1 << 5;

	dma2->cselr.ch7.cs = 4; // Select 0100:LPUART_RX

	dma2->ch7.ccr.en = 1;
#elif defined(CONF_L0)
	dma1->ch3.ccr = ccr;
	nvic_iser[0] |= 1 << 10;

	dma1->cselr.ch3.cs = 5; // Select 0101:LPUART1_RX

	dma1->ch3.ccr.en = 1;
#endif
	lpuart1->cr1.re = 1;
	while (!lpuart1->isr.reack);
}

extern void assert(bool); // main.c
extern void cmd_rotate(); // cmd.c

void ic_dma_receiver()
{
//	io_green->odr.pin_green ^= 1;
#ifdef CONF_L4
	dma2->ifcr.ch7.ctcif = 1;
#elif defined(CONF_L0)
	dma1->ifcr.ch3.ctcif = 1;
#endif
	cmd_rotate();
}

int uart_readbuf_length()
{
#ifdef CONF_L4
			return sizeof(uart_readbuf) - dma2->ch7.cndtr;
#elif defined(CONF_L0)
			return sizeof(uart_readbuf) - dma1->ch3.cndtr.ndt;
#endif
}

void i_lpuart1()
{
#ifdef CONF_L4
	// lpuart1->isr.cmf		// character match flag
	// lpuart1->icr.cmcf = 1	// reset character match flag
	assert(lpuart1->isr.cmf); // character match flag
	lpuart1->icr.cmcf = 1; // reset character match flag
	io_green->odr.pin_green ^= 1;
#endif
}

#endif
