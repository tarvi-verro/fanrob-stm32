
scripts/font-tiny.h: font-tiny.xcf | .aux
	@echo "compiling $<..."
	@echo "(define (convert-xcf-to-header filename outfile) \
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
	(gimp-quit 0)" | gimp -i -b - &> .aux/gimp-log && true # gimp is whiny, so give it a log file


font-bin.o: scripts/font-tiny.h font-tiny.xcf
	$(HOSTCC) scripts/font-conv.c -o scripts/font-conv \
		&& scripts/font-conv > .aux/font-tiny.bin
	@echo "gcc -c -x assembler-with-cpp -o \"$@\" -"
	@printf ".global font_bin\n.section .rodata\n.type font_bin, %%object\nfont_bin:\n\t.incbin \".aux/font-tiny.bin\"\n\t.align 4\n.size font_bin, .-font_bin\n" \
		| $(CC) $(CFLAGS) -c -x assembler-with-cpp -o "$@" -
