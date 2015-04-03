
CC:=arm-none-eabi-gcc
AR:=arm-none-eabi-ar
LD:=arm-none-eabi-ld

all: prg

SRC:=main.c early.S early_reset.c
OBJ:=$(patsubst %.c, %.o, $(filter %.c, $(SRC))) \
	$(patsubst %.S, %.o, $(filter %.S, $(SRC)))

# Makes the .d header dependency connections
MKDEP = $(CC) -MM -MG $(CFLAGS) "$*.c" \
	| sed -e "s@^\(.*\)\:@$*.d $*.o\:@" > $@

CFLAGS:=$(CFLAGS) -g3 -O0 -mthumb -mcpu=cortex-m0 -fno-stack-protector \
	-ffunction-sections -fdata-sections -fmessage-length=0 -Wall -fno-builtin -Wa,-mcpu=cortex-m0,-mthumb,-EL

LDFLAGS:=$(LDFLAGS) -Bstatic -lgcc --specs=nosys.specs -mcpu=cortex-m0 \
	-mthumb

%.o: %.S
	$(CC) $(CFLAGS) $(LDFLAGS) -x assembler $*.S -c -o "$*.o"

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.d: %.c
	@$(MKDEP)

#-include $(SRC:%.c=%.d)

prg: $(OBJ) linker.ld
	$(CC) $(LDFLAGS) $(CFLAGS) $(OBJ) -T linker.ld -o $@

