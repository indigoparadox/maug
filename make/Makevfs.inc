
# vim: ft=make noexpandtab

MVFS_H_DIR := obj/mvfs
MVFS_DEFINE := MVFS_ENABLED

define MVFS

$$(MVFS_H_DIR)/mvfs.h: $(1)
	mkdir -p obj/mvfs

	# Add include guard.
	echo "#ifndef MVFS_H\n#define MVFS_H\n" > $$@

	for f in $$^; do \
		xxd -i "$$$$f" >> $$@ ; \
	done

	# Add a directory of file contents.
	echo "\nstatic unsigned char* gc_mvfs_data[] = {" >> $$@
	for f in $$^; do \
		b="`echo -n $$$$f | tr -c '[:alnum:]' '_'`" ; \
		echo "   $$$$b," >> $$@ ; \
	done
	echo "   (void*)0\n};" >> $$@

	# Add a directory of file sizes.
	echo "\nstatic unsigned int* gc_mvfs_sizes[] = {" >> $$@
	for f in $$^; do \
		b="`echo -n $$$$f | tr -c '[:alnum:]' '_'`" ; \
		echo "   &$$$${b}_len," >> $$@ ; \
	done
	echo "   (void*)0\n};" >> $$@

	# Add a directory of filenames.
	echo "\nstatic char* gc_mvfs_filenames[] = {" >> $$@
	for f in $$^; do \
		echo "   \"$$$$f\"," >> $$@ ; \
	done
	echo "   \"\"\n};" >> $$@

	# Close include guard.
	echo "\n#endif" >> $$@

CFLAGS_OPT_GCC += -D$$(MVFS_DEFINE) -I$$(MVFS_H_DIR)
CFLAGS_OPT_WATCOM += -D$$(MVFS_DEFINE) -i=$$(MVFS_H_DIR)
MAUG_DEPS += $$(MVFS_H_DIR)/mvfs.h

endef


