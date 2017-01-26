
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
O:=build
endif

ifndef PRINT_PRETTY
PRINT_PRETTY:=1
endif

all: $O/prg

chkpath:
	@[ -d $(PATH_LIBGCC) ]
	@[ -d $(PATH_LIBNEW) ]

SRC:=main.c early.S early_reset.c lcd-com.c font-lookup.c clock.c app-info.c \
	app.c app-menu.c app-exposure.c kbd.c camsig.c app-heater.c heater.c \
	decimal.c tim1-en.c app-voltm.c adc.c rcc.c uart.c fanctl.c cmd.c

OBJ:=$O/font-bin.o \
	$(patsubst %.c, $O/%.o, $(filter %.c, $(SRC))) \
	$(patsubst %.S, $O/%.o, $(filter %.S, $(SRC)))


cflags:=$(CFLAGS) -g3 -O0 -mthumb -mcpu=cortex-m0 -fno-stack-protector \
	-ffunction-sections -fdata-sections -fmessage-length=0 -Wall \
	-fno-builtin -Wa,-mcpu=cortex-m0,-mthumb,-EL

ASFLAGS:= -g3 -mthumb -mcpu=cortex-m0 -EL

LDFLAGS:=$(LDFLAGS) \
	-L$(PATH_LIBGCC) \
	-L$(PATH_LIBNEW) \
	-lgcc -lc -lnosys

$O:
	@mkdir $O

$O/%.o: %.S | $O
	@mkdir -p $(@D)
ifeq ($(PRINT_PRETTY), 1)
	@printf "  AS\t$@\n"
	@$(AS) $(ASFLAGS) -c $< -o "$@"
else
	$(AS) $(ASFLAGS) -c $< -o "$@"
endif

$O/%.o: %.c | $O
	@mkdir -p $(@D)
ifeq ($(PRINT_PRETTY), 1)
	@printf "  CC\t$@\n"
	@$(CC) $(cflags) -c $< -o $@
else
	$(CC) $(cflags) -c $< -o $@
endif

# Makes the .d header dependency connections
$O/%.d: %.c | $O
	@mkdir -p $(@D)
	@printf $*.d\ > $@; $(CC) -MM -MG $(cflags) "$*.c" >> $@

ifneq ($(MAKECMDGOALS),clean)
-include $(patsubst %.c, %.d, $(filter %.c, $(SRC)))
endif

include font-cc.mk

$O/.aux: | $O
	@mkdir $O/.aux

$O/prg: $(OBJ) linker.ld | chkpath
ifeq ($(PRINT_PRETTY), 1)
	@printf "  LD\t$@\n"
	@$(LD) -T linker.ld -o $@ $(OBJ) $(LDFLAGS)
else
	$(LD) -T linker.ld -o $@ $(OBJ) $(LDFLAGS)
endif

clean::
	rm -rf $(OBJ) $O/prg

.PHONY: clean all chkpath
