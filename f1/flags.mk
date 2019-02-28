
B_F1:=$(patsubst %/,%,$(dir $(filter %/f1/flags.mk, $(MAKEFILE_LIST))))

CFLAGS_F1=-mthumb -mcpu=cortex-m3 -fno-stack-protector \
	-ffunction-sections -fdata-sections -fmessage-length=0 \
	-fno-builtin -Wa,-mcpu=cortex-m3,-mthumb,-EL -DCONF_F1 \
	-I"$(B_F1)" $(CFLAGS)

ASFLAGS_F1=-g3 -mthumb -mcpu=cortex-m3 -EL $(ASFLAGS)

LDFLAGS_F1=$(LDFLAGS)


$O/prg-f1: $(OBJ:%.o=%-f1.o) $O/$(B_F1)/early-f1.o $(B_F1)/linker.ld | chkpath
ifeq ($(PRINT_PRETTY), 1)
	@printf "  LD\t$@\n"
	@$(LD) -T $(B_F1)/linker.ld -o $@ $(filter-out $(B_F1)/linker.ld, $^) $(LDFLAGS_F1)
else
	$(LD) -T $(B_F1)/linker.ld -o $@ $(filter-out $(B_F1)/linker.ld, $^) $(LDFLAGS_F1)
endif

$O/prg-f1.bin: $O/prg-f1
ifeq ($(PRINT_PRETTY), 1)
	@printf "  OBJC\t$@\n"
	@$(OC) -O binary $< $@
else
	$(OC) -O binary $< $@
endif


$O/%-f1.o: %.c | $O
	@mkdir -p $(@D)
ifeq ($(PRINT_PRETTY), 1)
	@printf "  CC\t$@\n"
	@$(CC) $(CFLAGS_F1) -c $< -o $@ -MD -MD -MF $(basename $@).d
else
	$(CC) $(CFLAGS_F1) -c $< -o $@ -MD -MD -MF $(basename $@).d
endif

$O/%-f1.o: $O/%.S | $O
	@mkdir -p $(@D)
ifeq ($(PRINT_PRETTY), 1)
	@printf "  AS\t$@\n"
	@$(AS) $(ASFLAGS_F1) -c $< -o "$@"
else
	$(AS) $(ASFLAGS_F1) -c $< -o "$@"
endif

$O/%-f1.o: %.S | $O
	@mkdir -p $(@D)
ifeq ($(PRINT_PRETTY), 1)
	@printf "  AS\t$@\n"
	@$(AS) $(ASFLAGS_F1) -c $< -o "$@"
else
	$(AS) $(ASFLAGS_F1) -c $< -o "$@"
endif

ifneq ($(MAKECMDGOALS),clean)
-include $(patsubst %.c, $O/%-f1.d, $(filter %.c, $(SRC)))
endif

