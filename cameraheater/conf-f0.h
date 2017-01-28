
/* PC8 is the blue led */
#define io_blue gpio_reg_c
#define iop_blue_en iopcen
#define pin_blue pin8

#define io_green gpio_reg_c
#define iop_green_en iopcen
#define iop_green_rcc ahbenr
#define pin_green pin9

#define io_user gpio_reg_a
#define iop_user_en iopaen
#define pin_user pin0

/* output signal to camera */
#define io_camsig gpio_reg_c
#define iop_camsig_en iopcen
#define pin_camsig pin3

/* keybd; this is more hardcoded than it impresses */
#define io_kbd gpio_reg_c
#define iop_kbd_en iopcen
#define pin_up pin10
#define pin_down pin11
#define pin_left pin4
#define pin_right pin5

/* lcd stuff */
#define io_lcd gpio_reg_a
#define pin_lcd_vdd pin1
#define pin_lcd_res pin2
#define pin_lcd_dc pin3
#define pin_lcd_nss pin4
#define pin_lcd_sck pin5
#define pin_lcd_bg pin6
#define pin_lcd_mosi pin7

#define spi_lcd spi1_reg
#define ch_lcd ch3 /* DMA CH3 connects to spi1-tx */
#define ch_lcdbg ch1 /* TIM3 PWM channel */

/* heater wire */
#define io_heat gpio_reg_b
#define iop_heat_en iopben
#define pin_heat pin1

