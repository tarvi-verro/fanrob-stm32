
HOSTCC:=gcc

CC:=arm-none-eabi-gcc
AS:=arm-none-eabi-as
AR:=arm-none-eabi-ar
LD:=arm-none-eabi-ld
OC:=arm-none-eabi-objcopy

# Because we're in a subdirectory, O must be one down, otherwise object files
# start surfacing
O?=build/usbexample

PRINT_PRETTY?=1

f1: $O/prg-f1
f1-bin: $O/prg-f1.bin

SRC:=main.c ../f1/early.S ../usb.c ../assert.c ../rcc.c usb-pck.c

OBJ:= \
	$(patsubst %.c, $O/%.o, $(filter %.c, $(SRC))) \
	$(patsubst %.S, $O/%.o, $(filter %.S, $(SRC))) \
	$(patsubst %.xcf, $O/%.o, $(filter %.xcf, $(SRC)))

include ../f1/flags.mk

# PATH_LIBGCC is where libgcc.a is and PATH_LIBNEW points to the directory of
# libc.a, libm.a and libnosys.a. They should evaluate to something like:
#
#	LIBGCC: /usr/lib/gcc/arm-none-eabi/8.3.0/thumb/v7-m/nofp
#	LIBNEW: /usr/arm-none-eabi/lib/thumb/v7-m/nofp
#
PATH_LIBNEW?=$(eval PATH_LIBNEW:=$$(dir $$(abspath $$(shell \
	     $$(CC) $$(CFLAGS_F1) -print-file-name=libnosys.a \
	     ))))$(PATH_LIBNEW)
PATH_LIBGCC?=$(eval PATH_LIBGCC:=$$(dir $$(abspath $$(shell \
	     $$(CC) $$(CFLAGS_F1) -print-file-name=libgcc.a \
	     ))))$(PATH_LIBGCC)


CFLAGS:=$(CFLAGS) -g3 -O0  -Wall -I.. -I.

LDFLAGS:=$(LDFLAGS) \
	-L$(PATH_LIBGCC) \
	-L$(PATH_LIBNEW) \
	-lgcc -lc -lnosys

$O:
	@mkdir -p $O

$O/.aux: | $O
	@mkdir $O/.aux


clean::
	rm -rf $(OBJ) $O/prg

.PHONY: clean all chkpath f1 f1-bin

