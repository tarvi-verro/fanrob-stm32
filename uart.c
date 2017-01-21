#include "conf.h"
#include "lpuart.h"
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

	io_uart->afr.pin_uart_tx = 8;
	io_uart->afr.pin_uart_rx = 8;

	io_uart->ospeedr.pin_uart_tx = GPIO_OSPEEDR_MEDIUM;
	io_uart->ospeedr.pin_uart_rx = GPIO_OSPEEDR_MEDIUM;

	io_uart->otyper.pin_uart_tx = GPIO_OTYPER_PP;
	io_uart->otyper.pin_uart_rx = GPIO_OTYPER_OD;

	io_uart->pupdr.pin_uart_tx = GPIO_PUPDR_NONE;
	io_uart->pupdr.pin_uart_rx = GPIO_PUPDR_NONE;

	//io_uart->bsrr.reset.pin_uart_tx = 1;
	//io_uart->bsrr.reset.pin_uart_rx = 1;


}

void uart_print(char *s)
{
	for (; *s != '\0'; s++)
		uart_send_byte(*s);
}

void uart_send_byte(uint8_t byte)
{
	//while(!lpuart1->isr.idle);
	//while (!lpuart1->isr.fe);
	//while (!lpuart1->isr.pe);
	//while (!lpuart1->isr.nf);

//	lpuart1->cr1.te = 1;
	lpuart1->cr1.te = 1;
	//while (!lpuart1->isr.teack);
	while (!lpuart1->isr.txe);
	lpuart1->tdr.tdr = byte;
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

	// Set up break on '\n' to parse full lines
	lpuart1->cr2.add_0 = '\n' & 0x0f;
	lpuart1->cr2.add_1 = ('\n' & 0xf0) >> 4;
	lpuart1->cr1.cmie = 1; // enable interrupt
	nvic_iser[2] |= 1 << 6;


	// Set up DMA
	/*
	 * For L4 DMA2: (hw registers page 302)
	 *	   Request number | Channel 6	| Channel 7
	 *			4 | LPUART_TX	| LPUART_RX
	 */
	rcc->ahb1enr.dma2en = 1;

	dma2->ch7.cpar = &lpuart1->rdr; // Peripheral address
	dma2->ch7.cmar = uart_readbuf; // Memory address to push data to
	dma2->ch7.cndtr.ndt = sizeof(uart_readbuf) - 1; // Total number of data to be transferred

	for (int z = 0; z < 0x80000; z++) asm("nop");

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
	dma2->ch7.ccr = ccr;
	nvic_iser[2] |= 1 << 5;

	dma2->cselr.ch7.cs = 4; // Select 0100:LPUART_RX

	dma2->ch7.ccr.en = 1;
	interr_dma2_ch7();
	lpuart1->cr1.re = 1;
	while (!lpuart1->isr.reack);
}

extern void assert(bool); // main.c

void interr_dma2_ch7()
{
//	io_green->odr.pin_green ^= 1;
	dma2->ifcr.ch7.ctcif = 1;
}

void interr_lpuart1()
{

	// lpuart1->isr.cmf		// character match flag
	// lpuart1->icr.cmcf = 1	// reset character match flag
	assert(lpuart1->isr.cmf); // character match flag
	lpuart1->icr.cmcf = 1; // reset character match flag
	io_green->odr.pin_green ^= 1;
}
