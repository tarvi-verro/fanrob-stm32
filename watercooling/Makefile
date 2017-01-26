# TODO:
#  - refactor early.S (nvic table entries should be project indep)
#

HOSTCC:=gcc

CC:=arm-none-eabi-gcc
AS:=arm-none-eabi-as
AR:=arm-none-eabi-ar
LD:=arm-none-eabi-ld

# Where libgcc.a and the libnosys.a are, often times the version has to be
# bumped.
PATH_LIBGCC?=/usr/lib/gcc/arm-none-eabi/6.3.0/armv6-m

# Location of libc.a and libm.a.
PATH_LIBNEW?=/usr/arm-none-eabi/lib/armv6-m

ifndef O
# O must be one down, otherwise object files start surfacing
O:=build/watercooling
endif

ifndef PRINT_PRETTY
PRINT_PRETTY:=1
endif

l0: $O/prg-l0

chkpath:
	@[ -d $(PATH_LIBGCC) ]
	@[ -d $(PATH_LIBNEW) ]

SRC:=font-tiny.xcf main.c ../l0/early.S ../early_reset.c ../lcd-com.c \
	../font-lookup.c ../clock.c ../app-info.c ../app.c ../app-menu.c \
	../kbd.c ../decimal.c ../tim1-en.c ../app-voltm.c ../adc.c ../rcc.c \
	../uart.c fanctl.c ../cmd.c

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
include ../l0/flags.mk
include ../l4/flags.mk

$O/.aux: | $O
	@mkdir $O/.aux


clean::
	rm -rf $(OBJ) $O/prg

.PHONY: clean all chkpath
