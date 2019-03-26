

	.syntax unified
	.cpu cortex-m3
	.thumb

.global  Default_Handler
.global vecs


.section .text.Reset_Handler
.weak Reset_Handler
.type Reset_Handler, %function
Reset_Handler:
	ldr	r0, =_estack       /* set stack pointer */
	mov	sp, r0
/* copy data section to ram */
	ldr	r3, =_data_vma_s
	ldr	r1, =_data_lma
	ldr	r2, =_data_vma_e
CpyDat:
	ldr	r4, [r1]
	str	r4, [r3]
	adds	r1, r1, #4
	adds	r3, r3, #4
	cmp	r2, r3
	bhi	CpyDat
/*
	extern uint8_t __bss_start__, __bss_end__; */
/* null the bss */
	ldr	r1, =__bss_start__
	ldr	r2, =__bss_end__
	movs	r3, #0
	cmp	r1, r2
	beq	NoNull
NullBss:
	str	r3, [r1]
	adds	r1, r1, #4
	cmp	r1, r2
	bcc	NullBss
NoNull:
//	bl	early_reset
	bl	main
	bx	lr

.size Reset_Handler, .-Reset_Handler

.section .text.Default_Handler,"ax",%progbits
.weak Default_Handler
.type Default_Handler, %function
Default_Handler:
Infinite_Loop:
	movs	r0, #0
	bl	assert
	b	Infinite_Loop
.weak Hardfault_Handler
.type Hardfault_Handler, %function
Hardfault_Handler:
	movs	r0, #0
	bl	assert
	b Hardfault_Handler
.size Default_Handler, .-Default_Handler

// see 91010 memory registers @ 169
.section .vec_table,"a",%progbits
.type vecs, %object
vecs:
	.word _estack		// 0x00, reserved
	.word Reset_Handler	// 0x04, reset handler
	.word undef_interr	// 0x08, NMI
	.word Hardfault_Handler	// 0x0C, hardfault
	.word 0			// 0x10, MemManage
	.word 0			// 0x14, BusFault
	.word 0			// 0x18, UsageFault
	.word 0			// 0x1C, reserved
	.word 0			// 0x20, reserved
	.word 0			// 0x24, reserved
	.word 0			// 0x28, reserved
	.word undef_interr	// 0x2C, SVCall
	.word undef_interr	// 0x30, Debug Monitor
	.word 0			// 0x34, reserved
	.word undef_interr	// 0x38, PendSV
	.word undef_interr	// 0x3C, SysTick
	.word undef_interr	// 0x40, WWDG			1 << 0, [0]
	.word undef_interr	// 0x44, PVD
	.word undef_interr	// 0x48, TAMPER			1 << 2
	.word undef_interr	// 0x4C, RTC
	.word undef_interr	// 0x50, FLASH
	.word i_rtc_global	// 0x54, RCC			1 << 5
	.word i_exti_0		// 0x58, EXTI_L0
	.word i_exti_1		// 0x5C, EXTI_L1		1 << 7
	.word i_exti_2		// 0x60, EXTI_L2
	.word i_exti_3		// 0x64, EXTI_L3
	.word i_exti_4		// 0x68, EXTI_L4
	.word undef_interr	// 0x6C, DMA1_CH1
	.word undef_interr	// 0x70, DMA1_CH2
	.word undef_interr	// 0x74, DMA1_CH3
	.word undef_interr	// 0x78, DMA1_CH4
	.word undef_interr	// 0x7C, DMA1_CH5
	.word undef_interr	// 0x80, DMA1_CH6
	.word undef_interr	// 0x84, DMA1_CH7
	.word undef_interr	// 0x88, ADC1_2
	.word i_usbhp_cantx	// 0x8C, CAN1_TX		1 << 19, [0]
	.word i_usblp_canrx0	// 0x90, CAN1_RX0		1 << 20, [0]
	.word undef_interr	// 0x94, CAN1_RX1
	.word undef_interr	// 0x98, CAN1_SCE
	.word i_exti_9_5	// 0x9C, EXTI9_5 EXTI Line[9:5]
	.word undef_interr	// 0xA0, TIM1_BRK
	.word undef_interr	// 0xA4, TIM1_UP
	.word undef_interr	// 0xA8, TIM1_TRG_COM
	.word undef_interr	// 0xAC, TIM1_CC
	.word undef_interr	// 0xB0, TIM2
	.word undef_interr	// 0xB4, TIM3
	.word undef_interr	// 0xB8, TIM4
	.word undef_interr	// 0xBC, I2C1_EV
	.word undef_interr	// 0xC0, I2C1_ER		1 << 0, [1]
	.word undef_interr	// 0xC4, I2C2_EV
	.word undef_interr	// 0xC8, I2C2_ER
	.word undef_interr	// 0xCC, SPI1
	.word undef_interr	// 0xD0, SPI2
	.word i_usart1		// 0xD4, USART1			1 << 5, [1]
	.word undef_interr	// 0xD8, USART2
	.word undef_interr	// 0xDC, USART3
.size vecs, .-vecs

	.weak undef_interr
	.thumb_set undef_interr,Default_Handler

	.weak i_rtc_global
	.thumb_set i_rtc_global,Default_Handler

	.weak i_exti_0
	.thumb_set i_exti_0,Default_Handler

	.weak i_exti_1
	.thumb_set i_exti_1,Default_Handler

	.weak i_exti_2
	.thumb_set i_exti_2,Default_Handler

	.weak i_exti_3
	.thumb_set i_exti_3,Default_Handler

	.weak i_exti_4
	.thumb_set i_exti_4,Default_Handler

	.weak i_exti_9_5
	.thumb_set i_exti_9_5,Default_Handler

	.weak i_usbhp_cantx
	.thumb_set i_usbhp_cantx,Default_Handler

	.weak i_usblp_canrx0
	.thumb_set i_usblp_canrx0,Default_Handler

	.weak i_usart1
	.thumb_set i_usart1,Default_Handler

