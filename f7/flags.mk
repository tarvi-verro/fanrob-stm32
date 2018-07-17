
B_F7:=$(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))

CFLAGS_F7=-mthumb -mcpu=cortex-m7 -fno-stack-protector \
	-ffunction-sections -fdata-sections -fmessage-length=0 \
	-fno-builtin -Wa,-mcpu=cortex-m7,-mthumb,-EL -DCONF_F7=1 \
	-I"$(B_F7)" $(CFLAGS)

ASFLAGS_F7=-g3 -mthumb -mcpu=cortex-m7 -EL $(ASFLAGS)

LDFLAGS_F7=$(LDFLAGS)


$O/prg-f7: $(OBJ:%.o=%-f7.o) $O/$(B_F7)/early-f7.o $(B_F7)/linker.ld | chkpath
ifeq ($(PRINT_PRETTY), 1)
	@printf "  LD\t$@\n"
	@$(LD) -T $(B_F7)/linker.ld -o $@ $(filter-out $(B_F7)/linker.ld, $^) $(LDFLAGS_F7)
else
	$(LD) -T $(B_F7)/linker.ld -o $@ $(filter-out $(B_F7)/linker.ld, $^) $(LDFLAGS_F7)
endif

$O/prg-f7.bin: $O/prg-f7
ifeq ($(PRINT_PRETTY), 1)
	@printf "  OBJC\t$@\n"
	@$(OC) -O binary $< $@
else
	$(OC) -O binary $< $@
endif


$O/%-f7.o: %.c | $O
	@mkdir -p $(@D)
ifeq ($(PRINT_PRETTY), 1)
	@printf "  CC\t$@\n"
	@$(CC) $(CFLAGS_F7) -c $< -o $@ -MD -MD -MF $(basename $@).d
else
	$(CC) $(CFLAGS_F7) -c $< -o $@ -MD -MD -MF $(basename $@).d
endif

$O/%-f7.o: $O/%.S | $O
	@mkdir -p $(@D)
ifeq ($(PRINT_PRETTY), 1)
	@printf "  AS\t$@\n"
	@$(AS) $(ASFLAGS_F7) -c $< -o "$@"
else
	$(AS) $(ASFLAGS_F7) -c $< -o "$@"
endif

$O/%-f7.o: %.S | $O
	@mkdir -p $(@D)
ifeq ($(PRINT_PRETTY), 1)
	@printf "  AS\t$@\n"
	@$(AS) $(ASFLAGS_F7) -c $< -o "$@"
else
	$(AS) $(ASFLAGS_F7) -c $< -o "$@"
endif

ifneq ($(MAKECMDGOALS),clean)
-include $(patsubst %.c, $O/%-f7.d, $(filter %.c, $(SRC)))
endif

