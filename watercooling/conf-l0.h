#include <stdint.h>


#ifdef CFG_UART
/*
 * For L0 DMA1 (hw registers page 245, 253-254)
 *	   Request number | Channel 2	| Channel 3
 *			5 | LPUART1_TX	| LPUART1_RX
 */
static const struct uart_configuration cfg_uart = {
	.tx = PA2, /* A7 → lpuart1 tx */
	.rx = PA3, /* A2 → lpuart1 rx */
	.alt = 6,
	.lpuart = lpuart1_macro,
	.dma = dma1_macro,
	.dma_sel_lpuart_rx = 5, // Select 0101:LPUART1_RX
	.dma_sel_lpuart_tx = 5, // LPUART1_TX
	.baud = 9600,
	.clksrc = RCC_CLKSRC_LSE,
};
/* Configure uart.c */
#define ic_dma_both i_dma1_ch2_3
#define dmarx_ch ch3
#define dmatx_ch ch2
#define TXBUF_SIZE 300
#endif


#ifdef CFG_FANCTL
static const struct fanctl_configuration cfg_fan = {
	.pwr_in = PB7, /* D4, 12V line on/off */
	.step = 2, /* the minmum duty step */

	.fans = {
		{ // fan1ctl
			.rpm = PA11, /* D10 */
			.ctl = PA8, /* D9 */
			.ctl_fast_ch = TIM_CH1,
			.ctl_tim_af = 5,
			.ctl_initial_duty = 169,
		}, { // fan2ctl
			.rpm = PB5, /* D11 */
			.ctl = PA1, /* A1 */
			.ctl_fast_ch = TIM_CH2,
			.ctl_tim_af = 2,
			.ctl_initial_duty = 169,
		}, { // fan3ctl
			.rpm = PA12, /* D2 */
			.ctl = PB0, /* D3 */
			.ctl_fast_ch = TIM_CH3,
			.ctl_tim_af = 5,
			.ctl_initial_duty = 169,
		}, { // fan4ctl
			.rpm = PB4, /* D12 */
			.ctl = PB1, /* D6 */
			.ctl_fast_ch = TIM_CH4,
			.ctl_tim_af = 5,
			.ctl_initial_duty = 169,
		},
	},
};
#endif


#ifdef CFG_FAST
static const struct tim_fast_configuration cfg_fast = {
	.tim = tim2_macro,
	.ch1 = {
		.out.ccs = TIM_CCS_OUT,
		.out.ocm = 6,
		.out.ocpe = 1,
	},
	.ch2 = {
		.out.ccs = TIM_CCS_OUT,
		.out.ocm = 6,
		.out.ocpe = 1,
	},
	.ch3 = {
		.out.ccs = TIM_CCS_OUT,
		.out.ocm = 6,
		.out.ocpe = 1,
	},
	.ch4 = {
		.out.ccs = TIM_CCS_OUT,
		.out.ocm = 6,
		.out.ocpe = 1,
	},
	.cc1e = 1,
	.cc2e = 1,
	.cc3e = 1,
	.cc4e = 1,
	.frequency = 25000,
};
#endif

#ifdef CFG_PUMP
static const struct pump_configuration cfg_pump = {
	.gaten = PB1,
	.gaten_af = 5,
	.fast_ch = TIM_CH4,
};
#endif

#ifdef CFG_ASSERT
static const struct assert_configuration cfg_assert = {
	.led = PB3, /* LD3, green led */
};
#endif


#ifdef CFG_ONEWIRE
static const struct onewire_configuration cfg_ow = {
	.power = PNONE,
	.tx = PA9,
	.tx_af = 4,
	.extiln = LINE26,
	.usart = usart2_macro,
};
#define ic_usart_ow i_usart2
#endif

#ifdef CFG_DYNAMIC
static struct dynamic_configuration cfg_dyn = {
	.temperature = { 0x4A, 0x03, 0x16, 0x05, 0x89, 0x2C, 0xFF, 0x28 },
	.fan_mobo = 0,
	.fan_water_top = 3,
	.fan_water_bot = 2,

	.water_rpm_rest = 585,
	.water_rpm_max = 900,

	.mobo_rpm_rest = 900,
	.mobo_rpm_max = 1800,

	.temp_high = 37000,
	.temp_low = 28000,
};
#endif
