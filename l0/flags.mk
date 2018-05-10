
B_L0:=$(patsubst %/,%,$(dir $(filter %/l0/flags.mk, $(MAKEFILE_LIST))))

CFLAGS_L0=-mthumb -mcpu=cortex-m0 -fno-stack-protector \
	-ffunction-sections -fdata-sections -fmessage-length=0 \
	-fno-builtin -Wa,-mcpu=cortex-m0,-mthumb,-EL -DCONF_L0 \
	-I"$(B_L0)" $(CFLAGS)

ASFLAGS_L0=-g3 -mthumb -mcpu=cortex-m0 -EL $(ASFLAGS)

LDFLAGS_L0=$(LDFLAGS)


$O/prg-l0: $(OBJ:%.o=%-l0.o) $O/$(B_L0)/early-l0.o $(B_L0)/linker.ld | chkpath
ifeq ($(PRINT_PRETTY), 1)
	@printf "  LD\t$@\n"
	@$(LD) -T $(B_L0)/linker.ld -o $@ $(filter-out $(B_L0)/linker.ld, $^) $(LDFLAGS_L0)
else
	$(LD) -T $(B_L0)/linker.ld -o $@ $(filter-out $(B_L0)/linker.ld, $^) $(LDFLAGS_L0)
endif

$O/prg-l0.bin: $O/prg-l0
ifeq ($(PRINT_PRETTY), 1)
	@printf "  OBJC\t$@\n"
	@$(OC) -O binary $< $@
else
	$(OC) -O binary $< $@
endif


$O/%-l0.o: %.c | $O
	@mkdir -p $(@D)
ifeq ($(PRINT_PRETTY), 1)
	@printf "  CC\t$@\n"
	@$(CC) $(CFLAGS_L0) -c $< -o $@ -MD -MD -MF $(basename $@).d
else
	$(CC) $(CFLAGS_L0) -c $< -o $@ -MD -MD -MF $(basename $@).d
endif

$O/%-l0.o: $O/%.S | $O
	@mkdir -p $(@D)
ifeq ($(PRINT_PRETTY), 1)
	@printf "  AS\t$@\n"
	@$(AS) $(ASFLAGS_L0) -c $< -o "$@"
else
	$(AS) $(ASFLAGS_L0) -c $< -o "$@"
endif

$O/%-l0.o: %.S | $O
	@mkdir -p $(@D)
ifeq ($(PRINT_PRETTY), 1)
	@printf "  AS\t$@\n"
	@$(AS) $(ASFLAGS_L0) -c $< -o "$@"
else
	$(AS) $(ASFLAGS_L0) -c $< -o "$@"
endif

ifneq ($(MAKECMDGOALS),clean)
-include $(patsubst %.c, $O/%-l0.d, $(filter %.c, $(SRC)))
endif

