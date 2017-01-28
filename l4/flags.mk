
B_L4:=$(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))

CFLAGS_L4=-mthumb -mcpu=cortex-m0 -fno-stack-protector \
	-ffunction-sections -fdata-sections -fmessage-length=0 \
	-fno-builtin -Wa,-mcpu=cortex-m0,-mthumb,-EL -DCONF_L4 \
	-I"$(B_L4)" $(CFLAGS)

ASFLAGS_L4=-g3 -mthumb -mcpu=cortex-m0 -EL $(ASFLAGS)

LDFLAGS_L4=$(LDFLAGS)


$O/prg-l4: $(OBJ:%.o=%-l4.o) $O/../l4/early-l4.o $(B_L4)/linker.ld | chkpath
ifeq ($(PRINT_PRETTY), 1)
	@printf "  LD\t$@\n"
	@$(LD) -T ../l4/linker.ld -o $@ $(filter-out ../l4/linker.ld, $^) $(LDFLAGS_L4)
else
	$(LD) -T ../l4/linker.ld -o $@ $(filter-out ../l4/linker.ld, $^) $(LDFLAGS_L4)
endif


$O/%-l4.o: %.c | $O
	@mkdir -p $(@D)
ifeq ($(PRINT_PRETTY), 1)
	@printf "  CC\t$@\n"
	@$(CC) $(CFLAGS_L4) -c $< -o $@ -MD -MD -MF $(basename $@).d
else
	$(CC) $(CFLAGS_L4) -c $< -o $@ -MD -MD -MF $(basename $@).d
endif

$O/%-l4.o: $O/%.S | $O
	@mkdir -p $(@D)
ifeq ($(PRINT_PRETTY), 1)
	@printf "  AS\t$@\n"
	@$(AS) $(ASFLAGS_L4) -c $< -o "$@"
else
	$(AS) $(ASFLAGS_L4) -c $< -o "$@"
endif

$O/%-l4.o: %.S | $O
	@mkdir -p $(@D)
ifeq ($(PRINT_PRETTY), 1)
	@printf "  AS\t$@\n"
	@$(AS) $(ASFLAGS_L4) -c $< -o "$@"
else
	$(AS) $(ASFLAGS_L4) -c $< -o "$@"
endif

ifneq ($(MAKECMDGOALS),clean)
-include $(patsubst %.c, $O/%-l4.d, $(filter %.c, $(SRC)))
endif

