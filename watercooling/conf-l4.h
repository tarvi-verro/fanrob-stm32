#include <stdint.h>


#ifdef CFG_UART
/*
 * For L4 DMA2: (hw registers page 302)
 *	   Request number | Channel 6	| Channel 7
 *			4 | LPUART_TX	| LPUART_RX
 */
static const struct uart_configuration cfg_uart = {
	.tx = PA2, /* A7 → lpuart1 tx */
	.rx = PA3, /* A2 → lpuart1 rx */
	.alt = 8,
	.lpuart = lpuart1_macro,
	.dma = dma2_macro,
	.dma_sel_lpuart_rx = 4, // Select 0100:LPUART_RX
};
/* Configure uart.c */
#define ic_dma_receiver i_dma2_ch7
#define dma_ch ch7
#endif


#ifdef CFG_FANCTL
static const struct fanctl_configuration cfg_fan = {
	.rpm = PB5, /* D11 */
	.ctl = PA11, /* D10 */
	.ctl_tim_af = 1,
	.ctl_tim_ch = TIM_CH4,
	.ctl_tim = tim1_macro,
};
#endif


#ifdef CFG_ASSERT
static const struct assert_configuration cfg_assert = {
	.led = PB3, /* LD3, green led */
};
#endif

