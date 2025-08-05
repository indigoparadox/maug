
# vim: ft=make noexpandtab

# TODO: Automatically use maug icon if no app icon present.

ifeq ("$(CC_GCC_WII)","")
	CC_GCC_WII := $(DEVKITPPC)/bin/powerpc-eabi-gcc
endif

ifeq ("$(ELF2DOL)","")
	ELF2DOL := $(DEVKITPRO)/tools/bin/elf2dol
endif

# Setup CFLAGS_GCC_WII

# Disable the keyboard and viewport/refresh functions, since the Wii doesn't
# have those.
DEFINES_GCC_WII += -D__WII__ -DRETROFLAT_OS_WII -DRETROFLAT_NO_KEYBOARD -DRETROFLAT_NO_VIEWPORT_REFRESH -DGEKKO
INCLUDES_GCC_WII += -I obj/mvfs

CFLAGS_GCC_WII := -mrvl -mcpu=750 -meabi -mhard-float $(DEFINES_GCC_WII) $(INCLUDES_GCC_WII)

# Setup CFLAGS_GCC_WII_LIBOGC

DEFINES_GCC_WII_LIBOGC += -DRETROFLAT_API_LIBOGC -DRETROFLAT_NO_CLI

INCLUDES_GCC_WII_LIBOGC += -I $(DEVKITPRO)/libogc/include \
	-I$(MAUG_ROOT)/api/retro2d/wii -I$(MAUG_ROOT)/api/input/wii \
	-I$(MAUG_ROOT)/api/file/unix -I$(MAUG_ROOT)/api/log/wii \
	-I$(MAUG_ROOT)/api/font/soft -I$(MAUG_ROOT)/api/mem/unix

CFLAGS_GCC_WII_LIBOGC := $(DEFINES_GCC_WII_LIBOGC) $(INCLUDES_GCC_WII_LIBOGC)

# Setup LDFLAGS_GCC_WII

LDFLAGS_GCC_WII := -mrvl -mcpu=750 -meabi -mhard-float

# Setup LDFLAGS_GCC_WII_LIBOGC

LIBS_GCC_WII_LIBOGC += -L$(DEVKITPRO)/libogc/lib/wii -lwiiuse -lbte -logc -lm

LDFLAGS_GCC_WII_LIBOGC := $(LIBS_GCC_WII_LIBOGC)

define TGTWIILIBO

# Remove XPM flags, as XPM mode is not supported on the DS.
CFLAGS_OPT_GCC_WII += \
	$$(subst -DRETROFLAT_XPM -I$$(RETROXPM_H_DIR),,$$(CFLAGS_OPT_GCC))

C_FILES_WII := $$(WIIASSET_C_FILE) $$(C_FILES)

MAUG_DEPS_WII := $$(subst $$(RETROXPM_H_DIR)/xpmasset.h,,$$(MAUG_DEPS))

ifeq ($$(FORCE_MVFS),)
ifeq ($$(MVFS_AVAIL),1)
# Selectively enable VFS for this platform.
$$(info Selectively enabling MVFS for platform "NDS" with no filesystem!)
CFLAGS_OPT_GCC_WII += -D$$(MVFS_DEFINE) -I$$(MVFS_H_DIR)
MAUG_DEPS_WII += $$(MVFS_H_DIR)/mvfs.h
endif
endif

ifeq ("$$(RETROFLAT_SOUND)","1")
	CFLAGS_GCC_WII += -I$(MAUG_ROOT)/api/sound/nds
endif

$(1).dol: obj/wii-libogc/$(1).elf
	$$(ELF2DOL) "$$<" "$$@"

obj/wii-libogc/$(1).elf: \
$$(addprefix obj/wii-libogc/,$$(subst .c,.o,$$(C_FILES_WII)))
	PATH="$(DEVKITPRO)/tools/bin:$(DEVKITPPC)/bin:$(PATH)" \
	$$(CC_GCC_WII) -o "$$@" $$^ \
		"-Wl,-Map,$$@.map" \
		$$(LIBS_GCC) $$(LDFLAGS_GCC_WII) $$(LDFLAGS_GCC_WII_LIBOGC)

obj/wii-libogc/%.o: %.c | $$(MAUG_DEPS_WII)
	$$(MD) $$(dir $$@)
	$$(CC_GCC_WII) -c $$< -o $$@ \
		$$(INCLUDES_GCC) $$(DEFINES_GCC) $$(GLOBAL_DEFINES) \
		$$(CFLAGS_GCC_WII) $$(CFLAGS_GCC_WII_LIBOGC) \
		$$(CFLAGS_OPT_GCC_WII)

CLEAN_TARGETS += $(1).nds

endef

