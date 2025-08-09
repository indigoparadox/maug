
# vim: ft=make noexpandtab

# TODO: Automatically use maug icon if no app icon present.

PREFIX_NX := aarch64-none-elf-

ifeq ("$(CC_GCC_NX)","")
CC_GCC_NX := $(DEVKITPRO)/devkitA64/bin/$(PREFIX_NX)gcc
endif

ifeq ("$(CC_GCC_NM_NX)","")
CC_GCC_NM_NX := $(DEVKITPRO)/devkitA64/bin/$(PREFIX_NX)gcc-nm
endif

ifeq ("$(CC_GPP_NX)","")
CC_GPP_NX := $(DEVKITPRO)/devkitA64/bin/$(PREFIX_NX)g++
endif

ifeq ("$(ELF2NRO)","")
ELF2NRO := $(DEVKITPRO)/tools/bin/elf2nro
endif

ifeq ("$(NACPTOOL)","")
NACPTOOL := $(DEVKITPRO)/tools/bin/nacptool
endif

ifeq ("$(NXPKGCONFIG)","")
PKGCONFIG_NX := $(DEVKITPRO)/portlibs/switch/bin/$(PREFIX_NX)pkg-config
endif

# Setup Switch-specific flags.

DEFINES_GCC_NX += -DRETROFLAT_OS_NX -DRETROFLAT_NO_VIEWPORT_REFRESH -D__SWITCH__

INCLUDES_GCC_NX += -I$(DEVKITPRO)/portlibs/switch/include -I$(DEVKITPRO)/libnx/include

CFLAGS_GCC_NX := $(DEFINES_GCC_NX) $(INCLUDES_GCC_NX) \
	-ffunction-sections -march=armv8-a+crc+crypto -mtune=cortex-a57 -mtp=soft -fPIE

LDFLAGS_GCC_NX := -specs=$(DEVKITPRO)/libnx/switch.specs -march=armv8-a+crc+crypto -mtune=cortex-a57 -mtp=soft -fPIE -L$(DEVKITPRO)/portlibs/switch/lib -L$(DEVKITPRO)/libnx/lib

define TGTNXSDL

# Setup CFLAGS_GCC_NX_SDL

DEFINES_GCC_NX_SDL += -DRETROFLAT_API_SDL2 -DRETROFLAT_NO_CLI

INCLUDES_GCC_NX_SDL += \
	-I$(MAUG_ROOT)/api/retro2d/sdl -I$(MAUG_ROOT)/api/input/sdl \
	-I$(MAUG_ROOT)/api/file/unix -I$(MAUG_ROOT)/api/log/unix \
	-I$(MAUG_ROOT)/api/font/soft -I$(MAUG_ROOT)/api/mem/unix

CFLAGS_GCC_NX_SDL := $$(DEFINES_GCC_NX_SDL) $$(INCLUDES_GCC_NX_SDL) \
	$$(shell $$(PKGCONFIG_NX) --cflags sdl2 SDL2_mixer SDL2_image)

# Setup LDFLAGS_GCC_NX_SDL

LIBS_GCC_NX_SDL += \
	$$(shell $$(PKGCONFIG_NX) --libs sdl2 SDL2_mixer SDL2_image SDL2_ttf)

LDFLAGS_GCC_NX_SDL := $$(LIBS_GCC_NX_SDL)

# Remove XPM flags, as XPM mode is not supported on the DS.
CFLAGS_OPT_GCC_NX += \
	$$(subst -DRETROFLAT_XPM -I$$(RETROXPM_H_DIR),,$$(CFLAGS_OPT_GCC))

C_FILES_NX := $$(NXASSET_C_FILE) $$(C_FILES)

MAUG_DEPS_NX := $$(subst $$(RETROXPM_H_DIR)/xpmasset.h,,$$(MAUG_DEPS))

ifeq ("$$(RETROFLAT_SOUND)","1")
	CFLAGS_GCC_NX += -I$(MAUG_ROOT)/api/sound/sdl
endif

ifneq ("$$(NX_ROMFS)","")
	NX_ROMFS_FLAG := --romfsdir=$$(NX_ROMFS)
endif

$(1).nro: obj/nx-sdl/$(1).elf | obj/nx-sdl/$(1).nacp
	$$(MD) $$(dir $$@)
	$$(ELF2NRO) $$< $$@ --icon=/opt/devkitpro/libnx/default_icon.jpg \
		--nacp=obj/nx-sdl/$(1).nacp $$(NX_ROMFS_FLAG)

obj/nx-sdl/$(1).nacp:
	$$(MD) $$(dir $$@)
	$$(NACPTOOL) --create "$(1)" "$(1)" "1.0.0" $$@

obj/nx-sdl/$(1).lst: obj/nx-sdl/$(1).elf
	$$(MD) $$(dir $$@)
	$$(CC_GCC_NM_NX) -CSn $$< > $$@

obj/nx-sdl/$(1).elf: \
$$(addprefix obj/nx-sdl/,$$(subst .c,.o,$$(C_FILES_NX)))
	$$(CC_GPP_NX) -o "$$@" $$^ \
		"-Wl,-Map,$$@.map" \
		$$(LIBS_GCC) $$(LDFLAGS_GCC_NX) $$(LDFLAGS_GCC_NX_SDL)

obj/nx-sdl/%.o: %.c | $$(MAUG_DEPS_NX)
	$$(MD) $$(dir $$@)
	$$(CC_GCC_NX) -c $$< -o $$@ \
		$$(INCLUDES_GCC) $$(DEFINES_GCC) $$(GLOBAL_DEFINES) \
		$$(CFLAGS_GCC_NX) $$(CFLAGS_GCC_NX_SDL) \
		$$(CFLAGS_OPT_GCC_NX)

CLEAN_TARGETS += $(1).nro

endef

