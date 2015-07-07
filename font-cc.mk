
ifndef GIMP
GIMP:=gimp
endif

MK_FONT_TINY= \
	echo "(define (convert-xcf-to-header filename outfile) \
		(let* ( \
			(image (car (gimp-file-load RUN-NONINTERACTIVE filename filename))) \
			(drawable (car (gimp-image-flatten image))) \
		) \
		(gimp-image-convert-rgb image) \
		(file-header-save RUN-NONINTERACTIVE image drawable outfile outfile) \
		(gimp-image-delete image) \
		) \
	) \
	(gimp-message-set-handler 1) \
	(convert-xcf-to-header \"$<\" \"$@\") \
	(gimp-quit 0)" | gimp -i -b - &> $O/.aux/gimp-log && true # gimp is whiny, so give it a log file

$O/scripts/font-tiny.h: font-tiny.xcf | $O/.aux
	@mkdir -p $(@D)
ifeq ($(PRINT_PRETTY),1)
	@printf "  GIMP\t$@\n"
	@$(MK_FONT_TINY)
else
	$(MK_FONT_TINY)
endif

$O/scripts/font-conv: scripts/font-conv.c $O/scripts/font-tiny.h
	@mkdir -p $(@D)
ifeq ($(PRINT_PRETTY),1)
	@printf "  HOSTC\t$@\n"
	@$(HOSTCC) -I "$O/scripts/" scripts/font-conv.c -o $@
else
	$(HOSTCC) -I "$O/scripts/" scripts/font-conv.c -o $@
endif

MK_FONT_BIN=\
	$O/scripts/font-conv > $O/.aux/font-tiny.bin && \
	printf ".global font_bin\n.section .rodata\n.type font_bin, %%object\nfont_bin:\n\t.incbin \"$O/.aux/font-tiny.bin\"\n\t.align 4\n.size font_bin, .-font_bin\n" \
		| $(AS) $(ASFLAGS) -c -o "$@" -

$O/font-bin.o: $O/scripts/font-conv | $O/.aux
ifeq ($(PRINT_PRETTY),1)
	@printf "  AS\t$@\n"
	@$(MK_FONT_BIN)
else
	$(MK_FONT_BIN)
endif

clean::
	rm -rf $O/.aux/font-tiny.bin $O/scripts/font-conv $O/font-bin.o \
		$O/scripts/font-tiny.h $O/.aux/gimp-log

