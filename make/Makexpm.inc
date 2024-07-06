
# vim: ft=make noexpandtab

define DIRTOXPMS

MAUG_XPM_H_DIR := obj/retroxpm

$$(MAUG_XPM_H_DIR)/xpmasset.h: $(1)
	mkdir -p $$(MAUG_XPM_H_DIR)

	# Add include guard.
	echo "#ifndef XPMASSET_H\n#define XPMASSET_H\n" > $$@

	echo "#ifdef XPMASSET_C\n" >> $$@

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

	echo "#else /* XPMASSET_C */\n" >> $$@

	# TODO: Extern declares?

	echo "#endif /* XPMASSET_C */\n" >> $$@

	# Close include guard.
	echo "#endif\n" >> $$@

GLOBAL_INCLUDES += $$(MAUG_XPM_H_DIR)
CLEAN_TARGETS += $$(MAUG_XPM_H_DIR)/xpmasset.h

endef

