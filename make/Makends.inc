
# vim: ft=make noexpandtab

ifeq ("$(CC_GCC_NDS)","")
	CC_GCC_NDS := arm-none-eabi-gcc
endif

ifeq ("$(NDSTOOL)","")
	NDSTOOL := ndstool
endif

ifeq ("$(CFLAGS_GCC_NDS)","")
	CFLAGS_GCC_NDS := -DRETROFLAT_OS_NDS -DARM9 --sysroot $(DEVKITARM)/arm-none-eabi -march=armv5te -mtune=arm946e-s -fomit-frame-pointer -ffast-math -mlittle-endian -I $(DEVKITPRO)/devkitARM/arm-none-eabi/include -I $(DEVKITPRO)/devkitARM/include
endif

ifeq ("$(CFLAGS_GCC_NDS_LIBNDS)","")
	CFLAGS_GCC_NDS_LIBNDS := -I$(DEVKITPRO)/libnds/include -DRETROFLAT_API_LIBNDS
endif

ifeq ("$(LDFLAGS_GCC_NDS)","")
	LDFLAGS_GCC_NDS := -specs=ds_arm9.specs -mlittle-endian
endif

ifeq ("$(LDFLAGS_GCC_NDS_LIBNDS)","")
	LDFLAGS_GCC_NDS_LIBNDS := -Wl,-Map,obj/nds-libnds/$(1).map -L$(DEVKITPRO)/libnds/lib -lnds9
endif

define TGTNDSLIBN

$(1).nds: obj/nds-libnds/$(1).elf
	$$(NDSTOOL) -c "$$@" -9 "$$<" -b "$(3)" "$(1);$(1);$(1)"

obj/nds-libnds/$(1).elf: $(addprefix obj/nds-libnds/,$(subst .c,.o,$(C_FILES)))
	PATH="$(DEVKITPPRO)/tools/bin:$(DEVKITPRO)/devkitARM/bin:$(PATH)" \
	$$(CC_GCC_NDS) -o $$@ $$^ \
		$$(LIBS_GCC) $$(LDFLAGS_GCC_NDS) $$(LDFLAGS_GCC_NDS_LIBNDS)

obj/nds-libnds/%.o: %.c | $(2)
	$$(MD) $$(dir $$@)
	$$(CC_GCC_NDS) -c -o $$@ $$< \
		$$(INCLUDES_GCC) $$(DEFINES_GCC) \
		$$(CFLAGS_GCC_NDS) $$(CFLAGS_GCC_NDS_LIBNDS)

CLEAN_TARGETS += $(1).nds

endef

