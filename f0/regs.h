#include <stdint.h>

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

