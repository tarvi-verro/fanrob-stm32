
B_F0:=$(patsubst %/,%,$(dir $(filter %/f0/flags.mk, $(MAKEFILE_LIST))))

CFLAGS_F0=-mthumb -mcpu=cortex-m0 -fno-stack-protector \
	-ffunction-sections -fdata-sections -fmessage-length=0 \
	-fno-builtin -Wa,-mcpu=cortex-m0,-mthumb,-EL -DCONF_F0 \
	-I"$(B_F0)" $(CFLAGS)

ASFLAGS_F0=-g3 -mthumb -mcpu=cortex-m0 -EL $(ASFLAGS)

LDFLAGS_F0=$(LDFLAGS)


$O/prg-f0: $(OBJ:%.o=%-f0.o) $O/../f0/early-f0.o $(B_F0)/linker.ld | chkpath
ifeq ($(PRINT_PRETTY), 1)
	@printf "  LD\t$@\n"
	@$(LD) -T ../f0/linker.ld -o $@ $(filter-out ../f0/linker.ld, $^) $(LDFLAGS_F0)
else
	$(LD) -T ../f0/linker.ld -o $@ $(filter-out ../f0/linker.ld, $^) $(LDFLAGS_F0)
endif


$O/%-f0.o: %.c | $O
	@mkdir -p $(@D)
ifeq ($(PRINT_PRETTY), 1)
	@printf "  CC\t$@\n"
	@$(CC) $(CFLAGS_F0) -c $< -o $@ -MD -MD -MF $(basename $@).d
else
	$(CC) $(CFLAGS_F0) -c $< -o $@ -MD -MD -MF $(basename $@).d
endif

$O/%-f0.o: $O/%.S | $O
	@mkdir -p $(@D)
ifeq ($(PRINT_PRETTY), 1)
	@printf "  AS\t$@\n"
	@$(AS) $(ASFLAGS_F0) -c $< -o "$@"
else
	$(AS) $(ASFLAGS_F0) -c $< -o "$@"
endif

$O/%-f0.o: %.S | $O
	@mkdir -p $(@D)
ifeq ($(PRINT_PRETTY), 1)
	@printf "  AS\t$@\n"
	@$(AS) $(ASFLAGS_F0) -c $< -o "$@"
else
	$(AS) $(ASFLAGS_F0) -c $< -o "$@"
endif

ifneq ($(MAKECMDGOALS),clean)
-include $(patsubst %.c, $O/%-f0.d, $(filter %.c, $(SRC)))
endif

