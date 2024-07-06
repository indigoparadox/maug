
# vim: ft=make noexpandtab

# Convert a directory of bitmaps to Wii TPL textures.

define DIRTOTPLS

$(1)_BITMAPS := $(wildcard $(1)/*)

$(1)/%.tpl: $(1)/%.bmp
	gxtexconv -i "$$<" -o "$$@"

$(2)/$(1)_tpl.h: $$(subst .bmp,.tpl,$$($(1)_BITMAPS))
	# Add include guard.
	echo "#ifndef $(1)_H\n#define $(1)_H\n" > $$@
	echo "#define RETROFLAT_TPL\n" >> $$@

	# Convert files into tpls and concat them into the header.
	for f in $$^; do \
		xxd -i "$$$$f" >> $$@ ; \
	done

	# Add a directory of TPL structs.
	echo "\nstatic char** gc_tpl_data[] = {" >> $$@
	for f in $$^; do \
		b="`basename "$$$$f" .tpl`" ; \
		echo "   $(1)_$$$${b}_tpl," >> $$@ ; \
	done
	echo "   (void*)0\n};" >> $$@

	# Add a directory of filenames.
	echo "\nstatic char* gc_tpl_filenames[] = {" >> $$@
	for f in $$^; do \
		b="`basename "$$$$f" .bmp`" ; \
		echo "   \"$$$$b\"," >> $$@ ; \
	done
	echo "   \"\"\n};" >> $$@

	# Close include guard.
	echo "\n#endif" >> $$@

endef


