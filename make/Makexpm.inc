
# vim: ft=make noexpandtab

define DIRTOXPMS

$(1)_BITMAPS := $(wildcard $(1)/*.bmp)

$(2)/$(1)_xpm.h: $$($(1)_BITMAPS)
	# Add include guard.
	echo "#ifndef $(1)_H\n#define $(1)_H\n" > $$@

	echo "#ifdef $(1)_C\n" >> $$@

	# Convert files into xpms and concat them into the header.
	for f in $$^; do \
		b="`basename "$$$$f" .bmp`" ; \
		convert "$$$$f" xpm:- | \
			sed -e "s/^static char \*xpm__/MAUG_CONST char\* SEG_MCONST gc_xpm_data_$$$$b/g" >> $$@ ; \
	done

	# Add a directory of XPM structs.
	echo "\nMAUG_CONST char** SEG_MCONST gc_xpm_data[] = {" >> $$@
	for f in $$^; do \
		b="`basename "$$$$f" .bmp`" ; \
		echo "   gc_xpm_data_$$$$b," >> $$@ ; \
	done
	echo "   (void*)0\n};" >> $$@

	# Add a directory of filenames.
	echo "\nMAUG_CONST char* SEG_MCONST gc_xpm_filenames[] = {" >> $$@
	for f in $$^; do \
		b="`basename "$$$$f" .bmp`" ; \
		echo "   \"$$$$b\"," >> $$@ ; \
	done
	echo "   \"\"\n};" >> $$@

	echo "#else /* $(1)_C */\n" >> $$@

	# TODO: Extern declares?

	echo "#endif /* $(1)_C */\n" >> $$@

	# Close include guard.
	echo "#endif\n" >> $$@

MAUG_DEPS += $(2)/$(1)_xpm.h
CLEAN_TARGETS += $(2)/$(1)_xpm.h

endef

