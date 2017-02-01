#include <stdint.h>


#ifdef CFG_UART
static const struct uart_configuration cfg_uart = {
	.tx = PA2, /* A7 → lpuart1 tx */
	.rx = PA3, /* A2 → lpuart1 rx */
	.alt = 6,
};
/* Configure uart.c */
#define ic_dma_receiver i_dma1_ch2_3
#endif


#ifdef CFG_FANCTL
static const struct fanctl_configuration cfg_fan = {
	.rpm = PA12, /* D2 */
	.ctl = PB0, /* D3 */
	.ctl_tim_af = 5,
	.ctl_tim_ch = TIM_CH3,
	.ctl_tim = tim2_macro,
};
#endif


#ifdef CFG_ASSERT
static const struct assert_configuration cfg_assert = {
	.led = PB3, /* LD3, green led */
};
#endif

