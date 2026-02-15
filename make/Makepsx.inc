
# vim: ft=make noexpandtab

# TODO: Automatically use maug icon if no app icon present.

PSN00B_ROOT=/usr/mipsel-none-elf

ifeq ("$(CC_GCC_PSX)","")
CC_GCC_PSX := mipsel-none-elf-gcc
endif

ifeq ("$(CC_GPP_PSX)","")
CC_GPP_PSX := mipsel-none-elf-g++
endif

# Setup PSX-specific flags.

DEFINES_GCC_PSX += -DRETROFLAT_OS_PSX -DRETROFLAT_NO_VIEWPORT_REFRESH -DPSN00BSDK=1

INCLUDES_GCC_PSX += -I$(PSN00B_ROOT)/include/libpsn00b

CFLAGS_GCC_PSX := $(DEFINES_GCC_PSX) $(INCLUDES_GCC_PSX) \
	-Wa,--strip-local-absolute -ffreestanding -fno-builtin -nostdlib -fdata-sections -ffunction-sections -fsigned-char -fno-strict-overflow -fdiagnostics-color=always -msoft-float -march=r3000 -mtune=r3000 -mabi=32 -mno-mt -mno-llsc -O2 -G8 -fno-pic -mno-abicalls -mgpopt -mno-extern-sdata

LDFLAGS_GCC_PSX := -T$(PSN00B_ROOT)/lib/libpsn00b/ldscripts/exe.ld -nostdlib -Wl,-gc-sections -G8 -static -lgcc $(PSN00B_ROOT)/lib/libpsn00b/debug/libpsxgpu_exe_gprel.a $(PSN00B_ROOT)/lib/libpsn00b/debug/libpsxgte_exe_gprel.a $(PSN00B_ROOT)/lib/libpsn00b/debug/libpsxspu_exe_gprel.a $(PSN00B_ROOT)/lib/libpsn00b/debug/libpsxcd_exe_gprel.a $(PSN00B_ROOT)/lib/libpsn00b/debug/libpsxpress_exe_gprel.a $(PSN00B_ROOT)/lib/libpsn00b/debug/libpsxsio_exe_gprel.a $(PSN00B_ROOT)/lib/libpsn00b/debug/libpsxetc_exe_gprel.a $(PSN00B_ROOT)/lib/libpsn00b/debug/libpsxapi_exe_gprel.a $(PSN00B_ROOT)/lib/libpsn00b/debug/libsmd_exe_gprel.a $(PSN00B_ROOT)/lib/libpsn00b/debug/liblzp_exe_gprel.a $(PSN00B_ROOT)/lib/libpsn00b/debug/libc_exe_gprel.a -lgcc

define TGTPSXPSN

# Setup CFLAGS_GCC_PSX_PSN

DEFINES_GCC_PSX_PSN += -DRETROFLAT_OS_PSX -DRETROFLAT_API_PSN -DRETROFLAT_NO_CLI

INCLUDES_GCC_PSX_PSN += \
	-I$(MAUG_ROOT)/api/retro2d/psn00b -I$(MAUG_ROOT)/api/input/psn00b \
	-I$(MAUG_ROOT)/api/file/psn00b -I$(MAUG_ROOT)/api/log/psn00b \
	-I$(MAUG_ROOT)/api/font/soft -I$(MAUG_ROOT)/api/mem/unix \
	-I$(MAUG_ROOT)/api/serial/asn1

CFLAGS_GCC_PSX_PSN := $$(DEFINES_GCC_PSX_PSN) $$(INCLUDES_GCC_PSX_PSN)

# Remove XPM flags, as XPM mode is not supported on the DS.
#CFLAGS_OPT_GCC_PSX += \
#	$$(subst -DRETROFLAT_XPM -I$$(RETROXPM_H_DIR),,$$(CFLAGS_OPT_GCC))

C_FILES_PSX := $$(PSXASSET_C_FILE) $$(C_FILES)

#MAUG_DEPS_PSX := $$(subst $$(RETROXPM_H_DIR)/xpmasset.h,,$$(MAUG_DEPS))

ifeq ("$$(RETROFLAT_SOUND)","1")
	CFLAGS_GCC_PSX += -I$(MAUG_ROOT)/api/sound/psx
endif

obj/psx/$(1).elf: \
$$(addprefix obj/psx/,$$(subst .c,.o,$$(C_FILES_PSX)))
	$$(CC_GPP_PSX) -o "$$@" $$^ \
		$$(LIBS_GCC) $$(LDFLAGS_GCC_PSX) $$(LDFLAGS_GCC_PSX_PSN)

obj/psx/%.o: %.c | $$(MAUG_DEPS_PSX)
	$$(MD) $$(dir $$@)
	$$(CC_GCC_PSX) -c $$< -o $$@ \
		$$(INCLUDES_GCC) $$(DEFINES_GCC) $$(GLOBAL_DEFINES) \
		$$(CFLAGS_GCC_PSX) $$(CFLAGS_OPT_GCC_PSX) $$(CFLAGS_GCC_PSX_PSN)

$(1)psx.exe: obj/psx/$(1).elf
	elf2x -q $$< $$@

CLEAN_TARGETS += $(1)psx.iso $(1)psx.exe

endef

