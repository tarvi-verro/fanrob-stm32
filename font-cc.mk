
B:=$(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))

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
	(gimp-quit 0)" | gimp -i -b - &> $O/gimp-log && true # gimp is whiny, so give it a log file

$O/scripts/font-tiny.h: $B/font-tiny.xcf
	@mkdir -p $(@D)
ifeq ($(PRINT_PRETTY),1)
	@printf "  GIMP\t$@\n"
	@$(MK_FONT_TINY)
else
	$(MK_FONT_TINY)
endif

$O/scripts/font-conv: $B/scripts/font-conv.c $O/scripts/font-tiny.h
	@mkdir -p $(@D)
ifeq ($(PRINT_PRETTY),1)
	@printf "  HOSTC\t$@\n"
	@$(HOSTCC) -I "$O/scripts/" $< -o $@
else
	$(HOSTCC) -I "$O/scripts/" $< -o $@
endif

$O/font-tiny.bin: $O/scripts/font-conv
ifeq ($(PRINT_PRETTY),1)
	@printf "  PIPE\t$@\n"
	@$< > $@
else
	$< > $@
endif


ASGEN= \
	echo '?\
	.global font_bin ?\
	.section .rodata ?\
	.type font_bin, %object ?\
	font_bin: ?\
		.incbin "$<" ?\
		.align 4 ?\
	.size font_bin, .-font_bin' | tr '?' '\n' > $@

$O/font-tiny.S:  $O/font-tiny.bin
ifeq ($(PRINT_PRETTY),1)
	@printf "  ASGEN\t$@\n"
	@$(ASGEN)
else
	$(ASGEN)
endif


$O/font-tiny-%.o: $O/font-tiny.S

clean::
	rm -rf $O/font-tiny.bin $O/scripts/font-conv $O/font-tiny.o \
		$O/scripts/font-tiny.h $O/gimp-log

