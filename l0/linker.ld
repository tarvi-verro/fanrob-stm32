
/* Entry point */
ENTRY(Reset_Handler)

_estack = 0x20001FFC;

MEMORY {
	FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 0x8000 /* 32 "Kbytes" */
	RAM1 (wrx) : ORIGIN = 0x20000000, LENGTH = 0x2000 /* 8 "Kbytes" */
}

SECTIONS {
	/* apparently startup code? */
	.vec_table : {
		. = ALIGN(4);
		KEEP(*(.vec_table))
		. = ALIGN(4);
	} > FLASH

	.text : {
		. = ALIGN(4);
		*(.text)
		*(.text*)
		. = ALIGN(4);
	} > FLASH

	.early : {
		. = ALIGN(4);
		*(.early)
		. = ALIGN(4);
	}

	.rodata : {
		. = ALIGN(4);
		_rodata_exti_gpio_callb_s = .;
		*(.rodata.exti.gpio.callb*)
		_rodata_exti_gpio_callb_e = .;
		. = ALIGN(4);
		*(.rodata*)
		. = ALIGN(4);
	} > FLASH

	.bss : {
		. = ALIGN(4);
		__bss_start__ = .;
		*(.bss*)
		. = ALIGN(4);
		__bss_end__ = .;
	} > RAM1

	.data : {
		. = ALIGN(4);
		_data_vma_s = .;
		*(.data*)
		. = ALIGN(4);
		_data_vma_e = .;
	} > RAM1 AT > FLASH

	_data_lma = LOADADDR(.data);

	.init_array : {
		. = ALIGN(4);
		*(.init_array*)
		. = ALIGN(4);
	} > FLASH
	.fini_array : {
		. = ALIGN(4);
		*(.fini_array*)
		. = ALIGN(4);
	} > FLASH

}
