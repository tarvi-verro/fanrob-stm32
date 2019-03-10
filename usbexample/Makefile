
HOSTCC:=gcc

CC:=arm-none-eabi-gcc
AS:=arm-none-eabi-as
AR:=arm-none-eabi-ar
LD:=arm-none-eabi-ld
OC:=arm-none-eabi-objcopy

# Where libgcc.a is, often times the version has to be bumped.
PATH_LIBGCC?=/usr/lib/gcc/arm-none-eabi/8.3.0/thumb/v6-m/nofp

# Location of libc.a, libm.a and libnosys.a.
PATH_LIBNEW?=/usr/arm-none-eabi/lib/thumb/v6-m/nofp

# O must be one down, otherwise object files start surfacing
O?=build/usbexample

PRINT_PRETTY?=1

f1: $O/prg-f1
f1-bin: $O/prg-f1.bin

chkpath:
	@[ -d $(PATH_LIBGCC) ]
	@[ -d $(PATH_LIBNEW) ]

SRC:=main.c ../f1/early.S ../usb.c ../assert.c ../rcc.c usb-pck.c

OBJ:= \
	$(patsubst %.c, $O/%.o, $(filter %.c, $(SRC))) \
	$(patsubst %.S, $O/%.o, $(filter %.S, $(SRC))) \
	$(patsubst %.xcf, $O/%.o, $(filter %.xcf, $(SRC)))


CFLAGS:=$(CFLAGS) -g3 -O0  -Wall -I.. -I.

LDFLAGS:=$(LDFLAGS) \
	-L$(PATH_LIBGCC) \
	-L$(PATH_LIBNEW) \
	-lgcc -lc -lnosys

$O:
	@mkdir -p $O

include ../font-cc.mk
include ../f1/flags.mk

$O/.aux: | $O
	@mkdir $O/.aux


clean::
	rm -rf $(OBJ) $O/prg

.PHONY: clean all chkpath f1 f1-bin

