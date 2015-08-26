
HOSTCC:=gcc

CC:=arm-none-eabi-gcc
AS:=arm-none-eabi-as
AR:=arm-none-eabi-ar
LD:=arm-none-eabi-ld

ifndef O
O:=build
endif

ifndef PRINT_PRETTY
PRINT_PRETTY:=1
endif

all: $O/prg

SRC:=main.c early.S early_reset.c lcd-com.c font-lookup.c clock.c app-info.c \
	app.c app-menu.c app-exposure.c kbd.c camsig.c app-heater.c heater.c \
	tim3-en.c
OBJ:=$O/font-bin.o \
	$(patsubst %.c, $O/%.o, $(filter %.c, $(SRC))) \
	$(patsubst %.S, $O/%.o, $(filter %.S, $(SRC)))


CFLAGS:=$(CFLAGS) -g3 -O0 -mthumb -mcpu=cortex-m0 -fno-stack-protector \
	-ffunction-sections -fdata-sections -fmessage-length=0 -Wall \
	-fno-builtin -Wa,-mcpu=cortex-m0,-mthumb,-EL

ASFLAGS:= -g3 -mthumb -mcpu=cortex-m0 -EL

LDFLAGS:=$(LDFLAGS) \
	-L/usr/lib/gcc/arm-none-eabi/5.2.0/armv6-m \
	-L/usr/arm-none-eabi/lib/armv6-m \
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
	@$(CC) $(CFLAGS) -c $< -o $@
else
	$(CC) $(CFLAGS) -c $< -o $@
endif

# Makes the .d header dependency connections
$O/%.d: %.c | $O
	@mkdir -p $(@D)
	@printf $*.d\ > $@; $(CC) -MM -MG $(CFLAGS) "$*.c" >> $@

ifneq ($(MAKECMDGOALS),clean)
-include $(patsubst %.c, %.d, $(filter %.c, $(SRC)))
endif

include font-cc.mk

$O/.aux: | $O
	@mkdir $O/.aux

$O/prg: $(OBJ) linker.ld
ifeq ($(PRINT_PRETTY), 1)
	@printf "  LD\t$@\n"
	@$(LD) -T linker.ld -o $@ $(OBJ) $(LDFLAGS)
else
	$(LD) -T linker.ld -o $@ $(OBJ) $(LDFLAGS)
endif

clean::
	rm -rf $(OBJ) $O/prg

.PHONY: clean all

