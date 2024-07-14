
# vim: ft=make noexpandtab

ifeq ("$(CC_GCC_NDS)","")
	CC_GCC_NDS := arm-none-eabi-gcc
endif

ifeq ("$(NDSTOOL)","")
	NDSTOOL := ndstool
endif

# Setup CFLAGS_GCC_NDS

DEFINES_GCC_NDS += -DRETROFLAT_OS_NDS -DARM9 -DRETROFLAT_NO_KEYBOARD
INCLUDES_GCC_NDS += -I $(DEVKITPRO)/devkitARM/arm-none-eabi/include -I $(DEVKITPRO)/devkitARM/include -I obj/mvfs

CFLAGS_GCC_NDS := $(DEFINES_GCC_NDS) $(INCLUDES_GCC_NDS) --sysroot $(DEVKITARM)/arm-none-eabi -march=armv5te -mtune=arm946e-s -fomit-frame-pointer -ffast-math -mlittle-endian

# Setup CFLAGS_GCC_NDS_LIBNDS

DEFINES_GCC_NDS_LIBNDS += -DRETROFLAT_API_LIBNDS
INCLUDES_GCC_NDS_LIBNDS += -I$(DEVKITPRO)/libnds/include -I$(MAUG_ROOT)/api/nds

CFLAGS_GCC_NDS_LIBNDS := $(DEFINES_GCC_NDS_LIBNDS) $(INCLUDES_GCC_NDS_LIBNDS)

# Setup LDFLAGS_GCC_NDS

LDFLAGS_GCC_NDS := -specs=ds_arm9.specs -mlittle-endian $(LIBS_GCC_NDS)

# Setup LDFLAGS_GCC_NDS_LIBNDS

LIBS_GCC_NDS_LIBNDS += -L$(DEVKITPRO)/libnds/lib -lnds9

LDFLAGS_GCC_NDS_LIBNDS := -Wl,-Map,obj/nds-libnds/$(1).map $(LIBS_GCC_NDS_LIBNDS)

NDSASSET_H_DIR := obj/bmpnds
NDSASSET_C_FILE :=
NDSASSET_H_FILE :=

define TGTNDSLIBN

# Remove XPM flags, as XPM mode is not supported on the DS.
CFLAGS_OPT_GCC_NDS := \
	$$(subst -DRETROFLAT_XPM -I$$(RETROXPM_H_DIR),,$$(CFLAGS_OPT_GCC))

$(1).nds: obj/nds-libnds/$(1).elf
	$$(NDSTOOL) -c "$$@" -9 "$$<" -b "$(2)" "$(1);$(1);$(1)"

obj/nds-libnds/$(1).elf: \
$$(NDS_ASSET_C_FILE) \
$$(addprefix obj/nds-libnds/,$$(subst .c,.o,$$(C_FILES)))
	PATH="$(DEVKITPPRO)/tools/bin:$(DEVKITPRO)/devkitARM/bin:$(PATH)" \
	$$(CC_GCC_NDS) -o $$@ $$^ \
		$$(LIBS_GCC) $$(LDFLAGS_GCC_NDS) $$(LDFLAGS_GCC_NDS_LIBNDS)

obj/nds-libnds/%.o: %.c | \
$$(subst $$(RETROXPM_H_DIR)/xpmasset.h,,$$(MAUG_DEPS)) $$(NDSASSET_H_FILE)
	$$(MD) $$(dir $$@)
	$$(CC_GCC_NDS) -c -o $$@ $$< \
		-I $$(NDSASSET_H_DIR) \
		$$(INCLUDES_GCC) $$(DEFINES_GCC) $$(GLOBAL_DEFINES) \
		$$(CFLAGS_GCC_NDS) $$(CFLAGS_GCC_NDS_LIBNDS) \
		$$(CFLAGS_OPT_GCC_NDS)

CLEAN_TARGETS += $(1).nds

endef

# ===

define BMPTONDS

NDSASSET_C_FILE += $$(NDSASSET_H_DIR)/ndsasset.c
NDSASSET_H_FILE += $$(NDSASSET_H_DIR)/ndsasset.h

$$(NDSASSET_H_DIR)/ndsasset.h $$(NDSASSET_H_DIR)/ndsasset.c: $(1)
	mkdir -p $$(NDSASSET_H_DIR)

	echo "\n#ifndef NDSASSET_H\n#define NDSASSET_H" > \
		$$(NDSASSET_H_FILE)

	echo "\n#ifdef NDSASSET_C\n#define extern\n#endif" >> \
		$$(NDSASSET_H_FILE)

	echo "\n#include <stddef.h>\n" >> \
		$$(NDSASSET_H_FILE)

	echo "\n#define NDSASSET_C\n#include \"ndsasset.h\"" > \
		$$(NDSASSET_C_FILE)

	for f in $$^; do \
		grit $$$$f -gB8 -Mh2 -Mw2 -gTff55ff -ftc -fa -o $$(NDSASSET_H_FILE); \
	done

	# === Index Tiles ===

	echo "\nconst unsigned int* gc_ndsassets_tiles[] = {" >> \
		$$(NDSASSET_C_FILE)

	for f in $$(notdir $$(basename $$^)); do \
		echo "   $$$${f}Tiles," >> $$(NDSASSET_C_FILE); \
	done

	echo "};\n" >> $$(NDSASSET_C_FILE)

	# === Index Palettes ===

	echo "const unsigned short* gc_ndsassets_pals[] = {" >> $$(NDSASSET_C_FILE)
		
	for f in $$(notdir $$(basename $$^)); do \
		echo "   $$$${f}Pal," >> $$(NDSASSET_C_FILE); \
	done

	echo "};\n" >> $$(NDSASSET_C_FILE)

	# === Index Lens ===

	echo "const size_t gc_ndsassets_tiles_lens[] = {" >> $$(NDSASSET_C_FILE)
		
	for f in $$(notdir $$(basename $$^)); do \
		echo "   $$$${f}TilesLen," >> \
			$$(NDSASSET_C_FILE); \
	done

	echo "};\n" >> $$(NDSASSET_C_FILE)

	echo "const size_t gc_ndsassets_pals_lens[] = {" >> $$(NDSASSET_C_FILE)
		
	for f in $$(notdir $$(basename $$^)); do \
		echo "   $$$${f}PalLen," >> \
			$$(NDSASSET_C_FILE); \
	done

	echo "};\n" >> $$(NDSASSET_H_DIR)/ndsasset.c;

	# === Index Names ===

	# TODO

	# === Header ===

	echo "\nextern const unsigned int* gc_ndsassets_tiles[];" >> \
		$$(NDSASSET_H_FILE)
	echo "extern const unsigned short* gc_ndsassets_pals[];" >> \
		$$(NDSASSET_H_FILE)
	echo "extern const size_t gc_ndsassets_tiles_lens[];" >> \
		$$(NDSASSET_H_FILE)
	echo "extern const size_t gc_ndsassets_pals_lens[];" >> \
		$$(NDSASSET_H_FILE)

	echo "\n#endif /* NDSASSET_H */\n" >> $$(NDSASSET_H_FILE)

CFLAGS_GCC_NDS_LIBNDS += -DUSE_NDSASSET

CLEAN_TARGETS += $$(NDSASSET_H_DIR)/ndsasset.h

endef

