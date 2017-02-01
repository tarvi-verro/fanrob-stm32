/* conf for the STM32F0308Discovery */


#ifdef CFG_MAIN
static const struct main_configuration cfg_main = {
	.blue = PC8, /* PC8 is the blue led */
	.user = PA0, /* a blue pressable button */
};
#endif


#ifdef CFG_ASSERT
static const struct assert_configuration cfg_assert = {
	.led = PC9,
};
#endif


#ifdef CFG_KBD
static const struct kbd_configuration cfg_kbd = {
	.left = PC4,
	.down = PC11,
	.up = PC10,
	.right = PC5,
};
#endif


#ifdef CFG_LCD
static const struct lcd_configuration cfg_lcd = {
	.vdd = PA1,
	.res = PA2,
	.dc = PA3,

	.bg = PA6,
	.bg_tim_fast_ch = TIM_CH1,
	.gpio_af_bg = 1, /* tim3_ch1 */

	.sck = PA5,
	.nss = PA4,
	.mosi = PA7,
	.gpio_af_spi = 0, /* spi1 */

	.spi = spi1_macro,
	.dma = dma_macro,
};
#define ch_lcd ch3 /* DMA CH3 connects to spi1-tx */
#endif


#ifdef CFG_UART
static const struct uart_configuration cfg_uart = {
	.tx = PNONE,
	.rx = PNONE,
	.alt = 0,
};
#endif


#ifdef CFG_CAMSIG
static const struct camsig_configuration cfg_camsig = {
	.cam = PC3,
};
#endif


#ifdef CFG_HEAT
static const struct heater_configuration cfg_heat = {
	.mosfet = PB1,
	.gpio_af_tim = 0,
	.mosfet_tim_slow_ch = TIM_CH1,

	.battery = PNONE,
};
#endif


#ifdef CFG_FAST
static const struct tim_fast_configuration cfg_fast = {
	.tim = tim3_macro,
	.ch1 = { /* LCD BG */
		.out.ccs = TIM_CCS_OUT,
		.out.ocm = 6, /* 110 -- pwm mode 1 */
		.out.ocpe = 1, /* preload enable */
	},
	.cc1e = 1,
};
#endif

#ifdef CFG_SLOW
static const struct tim_slow_configuration cfg_slow = {
	.tim = tim14_macro,
	.ch1 = { /* heater wire */
		.out.ccs = TIM_CCS_OUT,
		.out.ocm = 6, /* 110 -- pwm mode 1 */
		.out.ocpe = 1, /* preload enable */
	},
	.cc1e = 1,
};
#endif
