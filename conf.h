
/* PC8 is the blue led */
#define io_blue gpio_reg_c
#define iop_blue_en iopc_en
#define pin_blue pin8

#define io_green gpio_reg_c
#define iop_green_en iopc_en
#define pin_green pin9

#define io_user gpio_reg_a
#define iop_user_en iopa_en
#define pin_user pin0

#define io_lcd gpio_reg_a
#define pin_lcd_vdd pin1
#define pin_lcd_dc pin3
#define pin_lcd_res pin2
#define pin_lcd_mosi pin7
#define pin_lcd_sck pin5
#define pin_lcd_nss pin4
#define pin_lcd_bg pin6

#define spi_lcd spi1_reg

/* See DM00091010 - memory registers page 37 */
static volatile struct rcc_reg *const rcc = (struct rcc_reg *) 0x40021000;
static uint32_t *const gpioc = (uint32_t *) 0x48000800;
static uint32_t *const gpioa = (uint32_t *) 0x48000000;
static volatile struct gpio_reg *const gpio_reg_a = (struct gpio_reg *) 0x48000000;
static volatile struct gpio_reg *const gpio_reg_c = (struct gpio_reg *) 0x48000800;
static uint32_t *const tim1 = (uint32_t *) 0x40012C00;
static uint32_t *const exti = (uint32_t *) 0x40010400;
static uint32_t *const syscfg = (uint32_t *) 0x40010000;
static uint32_t *const nvic_iser = (uint32_t *) 0xe000e100;
static uint32_t *const nvic_ispr = (uint32_t *) 0xe000e200;
static uint32_t *const nvic_icpr = (uint32_t *) 0xe000e280;
//static uint32_t *const spi1 = (uint32_t *) 0x40013000;

static volatile struct spi_reg *const spi1_reg = (struct spi_reg *) 0x40013000;

