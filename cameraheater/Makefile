
HOSTCC:=gcc

CC:=arm-none-eabi-gcc
AS:=arm-none-eabi-as
AR:=arm-none-eabi-ar
LD:=arm-none-eabi-ld
OC:=arm-none-eabi-objcopy

# Where libgcc.a and the libnosys.a are, often times the version has to be
# bumped.
PATH_LIBGCC?=/usr/lib/gcc/arm-none-eabi/6.3.1/armv6-m

# Location of libc.a and libm.a.
PATH_LIBNEW?=/usr/arm-none-eabi/lib/armv6-m

ifndef O
O:=build/cameraheater
endif

ifndef PRINT_PRETTY
PRINT_PRETTY:=1
endif

l4: $O/prg-l4
l4-bin: $O/prg-l4.bin
f0: $O/prg-f0

chkpath:
	@[ -d $(PATH_LIBGCC) ]
	@[ -d $(PATH_LIBNEW) ]

SRC:=../font-tiny.xcf main.c ../early_reset.c ../lcd-com.c ../font-lookup.c \
	../clock.c ../app-info.c ../app.c ../app-menu.c app-exposure.c \
	../kbd.c camsig.c app-heater.c heater.c ../decimal.c ../app-voltm.c \
	../adc.c ../rcc.c ../uart.c ../assert.c ../tim-preset.c ../exti-abs.c \
	../cmd.c

OBJ:= \
	$(patsubst %.c, $O/%.o, $(filter %.c, $(SRC))) \
	$(patsubst %.S, $O/%.o, $(filter %.S, $(SRC))) \
	$(patsubst %.xcf, $O/%.o, $(filter %.xcf, $(SRC)))

# OTGT returns 'l4', 'f0', 'l0' from target name $@
#OTGT=$(lastword $(patsubst -, ,$(@:.o=)))

CFLAGS= -g3 -O0  -Wall -I.. -I.  -fstrict-aliasing -fstrict-volatile-bitfields

ASFLAGS:= -g3

LDFLAGS:=$(LDFLAGS) \
	-L$(PATH_LIBGCC) \
	-L$(PATH_LIBNEW) \
	-lgcc -lc -lnosys

$O:
	@mkdir -p $O


include ../f0/flags.mk
include ../l4/flags.mk
include ../font-cc.mk

$O/.aux: | $O
	mkdir -p $O/.aux


clean::
	find $O/.. -name '*.[do]' -exec rm '{}' \;
	find $O/.. -name 'prg-*' -exec rm '{}' \;

.PHONY: clean all chkpath f0 l4

