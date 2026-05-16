
# vim: ft=make noexpandtab

CC_SDCC := sdcc

LD_SDCC := sdcc

CFLAGS_SDCC := $(GLOBAL_DEFINES) $(INCLUDES_GCC)

define TGTGB

CFLAGS_SDCC_GB += -I$(MAUG_ROOT)/api/retro2d/null \
	-I$(MAUG_ROOT)/api/input/null -I$(MAUG_ROOT)/api/file/null \
	-I$(MAUG_ROOT)/api/log/null -I$(MAUG_ROOT)/api/font/soft \
	-I$(MAUG_ROOT)/api/mem/unix -I$(MAUG_ROOT)/api/serial/asn1

CFLAGS_SDCC_GB += -mhc08 -DMAUG_NO_STAT -D__BIG_ENDIAN__ -DMAUG_ANCIENT_C

LDFLAGS_SDCC_GB += 

ifeq ("$$(RETROFLAT_SOUND)","1")
	CFLAGS_SDCC_GB += \
		-I$(MAUG_ROOT)/api/sound/null -DRETROSND_SAMPLE_22050
endif

$(1).gb: $$(addprefix obj/gb/,$$(subst .c,.o,$$(C_FILES) $$(C_MAUG_STUBS)))
	$$(LD_SDCC)

obj/gb/%.o: %.c | $$(MAUG_DEPS_GB)
	$$(MD) $$(dir $$@)
	$$(CC_SDCC) \
		$$(CFLAGS_SDCC) $$(CFLAGS_SDCC_DOS) $$(CFLAGS_SDCC_GB) \
		$$(CFLAGS_OPT_SDCC_BIOS) \
		-o $$@ $$<

CLEAN_TARGETS += $(1).gb

endef

