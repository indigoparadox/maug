
# vim: ft=make noexpandtab

ifeq ("$(CC_GCC_NDS)","")
	CC_GCC_NDS := arm-none-eabi-gcc
endif

ifeq ("$(NDSTOOL)","")
	NDSTOOL := ndstool
endif

# Setup CFLAGS_GCC_NDS

DEFINES_GCC_NDS += -DRETROFLAT_OS_NDS -DARM9
INCLUDES_GCC_NDS += -I $(DEVKITPRO)/devkitARM/arm-none-eabi/include -I $(DEVKITPRO)/devkitARM/include

CFLAGS_GCC_NDS := $(DEFINES_GCC_NDS) $(INCLUDES_GCC_NDS) --sysroot $(DEVKITARM)/arm-none-eabi -march=armv5te -mtune=arm946e-s -fomit-frame-pointer -ffast-math -mlittle-endian

# Setup CFLAGS_GCC_NDS_LIBNDS

DEFINES_GCC_NDS_LIBNDS += -DRETROFLAT_API_LIBNDS
INCLUDES_GCC_NDS_LIBNDS += -I$(DEVKITPRO)/libnds/include

CFLAGS_GCC_NDS_LIBNDS := $(DEFINES_GCC_NDS_LIBNDS) $(INCLUDES_GCC_NDS_LIBNDS)

# Setup LDFLAGS_GCC_NDS

LDFLAGS_GCC_NDS := -specs=ds_arm9.specs -mlittle-endian $(LIBS_GCC_NDS)

# Setup LDFLAGS_GCC_NDS_LIBNDS

LIBS_GCC_NDS_LIBNDS += -L$(DEVKITPRO)/libnds/lib -lnds9

LDFLAGS_GCC_NDS_LIBNDS := -Wl,-Map,obj/nds-libnds/$(1).map $(LIBS_GCC_NDS_LIBNDS)

define TGTNDSLIBN

$(1).nds: obj/nds-libnds/$(1).elf
	$$(NDSTOOL) -c "$$@" -9 "$$<" -b "$(2)" "$(1);$(1);$(1)"

obj/nds-libnds/$(1).elf: $(addprefix obj/nds-libnds/,$(subst .c,.o,$(C_FILES)))
	PATH="$(DEVKITPPRO)/tools/bin:$(DEVKITPRO)/devkitARM/bin:$(PATH)" \
	$$(CC_GCC_NDS) -o $$@ $$^ \
		$$(LIBS_GCC) $$(LDFLAGS_GCC_NDS) $$(LDFLAGS_GCC_NDS_LIBNDS)

obj/nds-libnds/%.o: %.c | $(MAUG_DEPS)
	$$(MD) $$(dir $$@)
	$$(CC_GCC_NDS) -c -o $$@ $$< \
		$$(INCLUDES_GCC) $$(DEFINES_GCC) \
		$$(CFLAGS_GCC_NDS) $$(CFLAGS_GCC_NDS_LIBNDS)

CLEAN_TARGETS += $(1).nds

endef

