#include <stdint.h>

/* PC8 is the blue led */
#define io_blue gpio_reg_c
#define iop_blue_en iopc_en
#define pin_blue pin8

/* PB3 is LD3, green led */
#define io_green gpio_reg_b
#define iop_green_en iopb_en
#define pin_green pin3

#define io_user gpio_reg_a
#define iop_user_en iopa_en
#define pin_user pin0

/* output signal to camera */
#define io_camsig gpio_reg_c
#define iop_camsig_en iopc_en
#define pin_camsig pin3

/* keybd; this is more hardcoded than it impresses */
#define io_kbd gpio_reg_c
#define iop_kbd_en iopc_en
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
#define iop_heat_en iopb_en
#define pin_heat pin1

struct gpio_reg;
struct rcc_reg;
struct spi_reg;
struct dma_reg;
struct rtc_reg;
struct pwr_reg;
struct exti_reg;
struct syscfg_reg;
struct adc_reg;

/* See DM00091010 - memory registers page 37 */
/* Or  DM00088500 - Memory mapping on page 39 */
static volatile struct rcc_reg *const rcc = (struct rcc_reg *) 0x40021000;
static volatile struct rtc_reg *const rtc = (struct rtc_reg *) 0x40002800;
static volatile struct dma_reg *const dma = (struct dma_reg *) 0x40020000;
static volatile struct tim_reg *const tim3 = (struct tim_reg *) 0x40000400;
static volatile struct pwr_reg *const pwr = (struct pwr_reg *) 0x40007000;
static volatile struct adc_reg *const adc = (struct adc_reg *) 0x40012400;
static uint32_t *const gpioc = (uint32_t *) 0x48000800;
static uint32_t *const gpioa = (uint32_t *) 0x48000000;
static volatile struct gpio_reg *const gpio_reg_a = (struct gpio_reg *) 0x48000000;
static volatile struct gpio_reg *const gpio_reg_b = (struct gpio_reg *) 0x48000400;
static volatile struct gpio_reg *const gpio_reg_c = (struct gpio_reg *) 0x48000800;
static volatile struct tim_reg *const tim1 = (struct tim_reg *) 0x40012C00;
static volatile struct tim_reg *const tim14 = (struct tim_reg *) 0x40002000;
static volatile struct exti_reg *const exti = (struct exti_reg *) 0x40010400;
static uint32_t *const exti_int = (uint32_t *) 0x40010400;
static uint32_t *const syscfg_int = (uint32_t *) 0x40010000;
static struct syscfg_reg *const syscfg = (struct syscfg_reg *) 0x40010000;
static uint32_t *const nvic_iser = (uint32_t *) 0xe000e100;
static uint32_t *const nvic_ispr = (uint32_t *) 0xe000e200;
static uint32_t *const nvic_icpr = (uint32_t *) 0xe000e280;
static volatile struct spi_reg *const spi1_reg = (struct spi_reg *) 0x40013000;

