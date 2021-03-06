#pragma once
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
struct lpuart_reg;
struct usb_reg;

/* See DM00091010 - memory registers page 37 */
/* Or  DM00088500 - Memory mapping on page 39 */
static volatile struct rcc_reg *const rcc = (struct rcc_reg *) 0x40021000; // ✓
static volatile struct rtc_reg *const rtc = (struct rtc_reg *) 0x40002800; // ✓
static volatile struct dma_reg *const dma1 = (volatile struct dma_reg *) 0x40020000; // ✓
static volatile struct dma_reg *const dma2 = (struct dma_reg *) 0x40020400; // ✓
static volatile struct pwr_reg *const pwr = (struct pwr_reg *) 0x40007000;
static volatile struct adc_reg *const adc = (struct adc_reg *) 0x50040000; // ✓
static volatile struct gpio_reg *const gpio_reg_a = (struct gpio_reg *) 0x48000000; // ✓
static volatile struct gpio_reg *const gpio_reg_b = (struct gpio_reg *) 0x48000400; // ✓
static volatile struct gpio_reg *const gpio_reg_c = (struct gpio_reg *) 0x48000800; // ✓
static volatile struct gpio_reg *const gpio_reg_h = (struct gpio_reg *) 0x48001C00; // ✓
static volatile struct tim_reg *const tim1 = (struct tim_reg *) 0x40012C00; // ✓
static volatile struct tim_reg *const tim2 = (struct tim_reg *) 0x40000000; // ✓
static volatile struct tim_reg *const tim6 = (struct tim_reg *) 0x40001000; // ✓
static volatile struct tim_reg *const tim7 = (struct tim_reg *) 0x40001400; // ✓
static volatile struct tim_reg *const tim15 = (struct tim_reg *) 0x40014000; // ✓
static volatile struct tim_reg *const tim16 = (struct tim_reg *) 0x40014400; // ✓
static volatile struct exti_reg *const exti = (struct exti_reg *) 0x40010400; // ✓
static struct syscfg_reg *const syscfg = (struct syscfg_reg *) 0x40010000; // ✓
static uint32_t *const nvic_iser = (uint32_t *) 0xe000e100;
static uint32_t *const nvic_stir = (uint32_t *) 0xe000ef00;
static volatile struct spi_reg *const spi1 = (volatile struct spi_reg *) 0x40013000; // ✓
static struct lpuart_reg *const lpuart1 = (struct lpuart_reg *) 0x40008000; // ✓
static uint8_t *const usbsram = (uint8_t *) 0x40006C00; // ✓
static struct usb_reg *const usbfs = (struct usb_reg *) 0x40006800; // ✓

#define tim15_macro (struct tim_reg *) 0x40014000
#define tim16_macro (struct tim_reg *) 0x40014400
#define tim6_macro (struct tim_reg *) 0x40001000
#define tim2_macro (struct tim_reg *) 0x40000000
#define tim1_macro (struct tim_reg *) 0x40012C00
#define spi1_macro ((volatile struct spi_reg *) 0x40013000)
#define dma1_macro ((volatile struct dma_reg *) 0x40020000)
#define dma2_macro ((volatile struct dma_reg *) 0x40020000)
#define lpuart1_macro ((volatile struct lpuart_reg *) 0x40008000)
#define usbsram_macro ((uint8_t *) 0x40006C00)

