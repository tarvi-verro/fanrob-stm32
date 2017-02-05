/* conf for the NUCLEO(32)-L432KC */


#ifdef CFG_MAIN
static const struct main_configuration cfg_main = {
	.blue = PNONE,
	.user = PNONE,
};
#endif


#ifdef CFG_ASSERT
static const struct assert_configuration cfg_assert = {
	.led = PB3,
};
#endif


#ifdef CFG_KBD
static const struct kbd_configuration cfg_kbd = {
	.left = PA11,
	.down = PA8,
	.up = PB1,
	.right = PA7,
};
#endif


#ifdef CFG_LCD
static const struct lcd_configuration cfg_lcd = {
	.vdd = PB4,
	.res = PB6,
	.dc = PB7,

	.bg = PA10,
	.bg_tim_fast_ch = TIM_CH3,
	.gpio_af_bg = 1, /* tim1_ch3 */

	.sck = PA5,
	.nss = PA4,
	.mosi = PB5,
	.gpio_af_spi = 5, /* spi1 */

	.spi =  spi1_macro,
	.dma =  dma1_macro,
};
#define ch_lcd ch3 /* DMA CH3 connects to spi1-tx */
#endif


#ifdef CFG_UART
static const struct uart_configuration cfg_uart = {
	.tx = PA2,
	.rx = PA3,
	.alt = 8,
};
/* Configure uart.c */
#define ic_dma_receiver i_dma2_ch7
#endif


#ifdef CFG_CAMSIG
static const struct camsig_configuration cfg_camsig = {
	.cam = PA12,
};
#endif


#ifdef CFG_HEAT
static const struct heater_configuration cfg_heat = {
	.mosfet = PA6,
	.gpio_af_tim = 14,
	.mosfet_tim_slow_ch = TIM_CH1,

	.battery = PB0,
};
#endif


#ifdef CFG_FAST
static const struct tim_fast_configuration cfg_fast = {
	.tim = tim1_macro,
	.ch3 = { /* LCD BG */
		.out.ccs = TIM_CCS_OUT,
		.out.ocm = 6, /* 110 -- pwm mode 1 */
		.out.ocpe = 1, /* preload enable */
	},
	.cc3e = 1,
	.frequency = 200, // Hz
};
#endif

#ifdef CFG_SLOW
static const struct tim_slow_configuration cfg_slow = {
	.tim = tim16_macro,
	.ch1 = { /* heater wire */
		.out.ccs = TIM_CCS_OUT,
		.out.ocm = 6, /* 110 -- pwm mode 1 */
		.out.ocpe = 1, /* preload enable */
	},
	.cc1e = 1,
};
#endif
