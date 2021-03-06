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

/* See DM00091010 - memory registers page 37 */
/* Or  DM00088500 - Memory mapping on page 39 */
static volatile struct rcc_reg *const rcc = (struct rcc_reg *) 0x40021000; // ✓
static volatile struct rtc_reg *const rtc = (struct rtc_reg *) 0x40002800; // ✓
static volatile struct dma_reg *const dma1 = (struct dma_reg *) 0x40020000; // ✓
//static volatile struct dma_reg *const dma2 = (struct dma_reg *) 0x40020400; // ✕
static volatile struct pwr_reg *const pwr = (struct pwr_reg *) 0x40007000;
static volatile struct adc_reg *const adc = (struct adc_reg *) 0x40012400;
static volatile struct gpio_reg *const gpio_reg_a = (struct gpio_reg *) 0x50000000; // ✓
static volatile struct gpio_reg *const gpio_reg_b = (struct gpio_reg *) 0x50000400; // ✓
static volatile struct gpio_reg *const gpio_reg_c = (struct gpio_reg *) 0x50000800; // ✓
static volatile struct tim_reg *const tim2 = (struct tim_reg *) 0x40000000; // ✓
static volatile struct tim_reg *const tim3 = (struct tim_reg *) 0x40000400; // ✓
static volatile struct tim_reg *const tim6 = (struct tim_reg *) 0x40001000; // ✓
static volatile struct tim_reg *const tim7 = (struct tim_reg *) 0x40001400; // ✓
static volatile struct tim_reg *const tim21 = (struct tim_reg *) 0x40010800; // ✓
static volatile struct tim_reg *const tim22 = (struct tim_reg *) 0x40011400; // ✓
static volatile struct exti_reg *const exti = (struct exti_reg *) 0x40010400; // ✓
static struct syscfg_reg *const syscfg = (struct syscfg_reg *) 0x40010000; // ✓
static uint32_t *const nvic_iser = (uint32_t *) 0xe000e100; // ✓
//static uint32_t *const nvic_stir = (uint32_t *) 0xe000ef00; // ?
static volatile struct spi_reg *const spi1_reg = (struct spi_reg *) 0x40013000;
static struct lpuart_reg *const lpuart1 = (struct lpuart_reg *) 0x40004800; // ✓
static volatile struct usart_reg *const usart1 = (struct usart_reg *) 0x40013800; // ✓
static volatile struct usart_reg *const usart2 = (struct usart_reg *) 0x40004400; // ✓
static volatile struct usart_reg *const usart4 = (struct usart_reg *) 0x40004C00; // ✓
static volatile struct usart_reg *const usart5 = (struct usart_reg *) 0x40005000; // ✓

#define tim2_macro (struct tim_reg *) 0x40000000
#define tim3_macro (struct tim_reg *) 0x40000400
#define tim6_macro (struct tim_reg *) 0x40001000
#define tim7_macro (struct tim_reg *) 0x40001400
#define tim21_macro (struct tim_reg *) 0x40010800
#define tim22_macro (struct tim_reg *) 0x40011400
#define usart1_macro (struct usart_reg *) 0x40013800
#define usart2_macro (struct usart_reg *) 0x40004400
#define usart4_macro (struct usart_reg *) 0x40004C00
#define usart5_macro (struct usart_reg *) 0x40005000
#define lpuart1_macro (struct lpuart_reg *) 0x40004800
#define dma1_macro (struct dma_reg *) 0x40020000

