
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

DEFINES_GCC_PSX += -DRETROFLAT_OS_PSX -DRETROFLAT_NO_VIEWPORT_REFRESH -DPSN00BSDK=1 -DNDEBUG=1

INCLUDES_GCC_PSX += -I$(PSN00B_ROOT)/include/libpsn00b

CFLAGS_GCC_PSX := $(DEFINES_GCC_PSX) $(INCLUDES_GCC_PSX) \
	-Wa,--strip-local-absolute -ffreestanding -fno-builtin -nostdlib -fdata-sections -ffunction-sections -fsigned-char -fno-strict-overflow -fdiagnostics-color=always -msoft-float -march=r3000 -mtune=r3000 -mabi=32 -mno-mt -mno-llsc -O2 -G8 -fno-pic -mno-abicalls -mgpopt -mno-extern-sdata

LDFLAGS_GCC_PSX_FRONT := -T$(PSN00B_ROOT)/lib/libpsn00b/ldscripts/exe.ld -nostdlib -Wl,-gc-sections -G8 -static
LDFLAGS_GCC_PSX_BACK := -lgcc $(PSN00B_ROOT)/lib/libpsn00b/debug/libpsxgpu_exe_gprel.a $(PSN00B_ROOT)/lib/libpsn00b/debug/libpsxgte_exe_gprel.a $(PSN00B_ROOT)/lib/libpsn00b/debug/libpsxspu_exe_gprel.a $(PSN00B_ROOT)/lib/libpsn00b/debug/libpsxcd_exe_gprel.a $(PSN00B_ROOT)/lib/libpsn00b/debug/libpsxpress_exe_gprel.a $(PSN00B_ROOT)/lib/libpsn00b/debug/libpsxsio_exe_gprel.a $(PSN00B_ROOT)/lib/libpsn00b/debug/libpsxetc_exe_gprel.a $(PSN00B_ROOT)/lib/libpsn00b/debug/libpsxapi_exe_gprel.a $(PSN00B_ROOT)/lib/libpsn00b/debug/libsmd_exe_gprel.a $(PSN00B_ROOT)/lib/libpsn00b/debug/liblzp_exe_gprel.a $(PSN00B_ROOT)/lib/libpsn00b/debug/libc_exe_gprel.a -lgcc

define TGTPSXPSN

# Setup CFLAGS_GCC_PSX_PSN

DEFINES_GCC_PSX_PSN += -DRETROFLAT_OS_PSX -DRETROFLAT_API_PSN -DRETROFLAT_NO_CLI

INCLUDES_GCC_PSX_PSN += \
	-I$(MAUG_ROOT)/api/retro2d/psn00b -I$(MAUG_ROOT)/api/input/psn00b \
	-I$(MAUG_ROOT)/api/log/psn00b -I$(MAUG_ROOT)/api/font/soft \
	-I$(MAUG_ROOT)/api/mem/unix -I$(MAUG_ROOT)/api/serial/asn1

CFLAGS_GCC_PSX_PSN := $$(DEFINES_GCC_PSX_PSN) $$(INCLUDES_GCC_PSX_PSN)

# Remove XPM flags, as XPM mode is not supported on the DS.
#CFLAGS_OPT_GCC_PSX += \
#	$$(subst -DRETROFLAT_XPM -I$$(RETROXPM_H_DIR),,$$(CFLAGS_OPT_GCC))

C_FILES_PSX := $$(PSXASSET_C_FILE) $$(C_FILES)

ifeq ("$$(RETROFLAT_SOUND)","1")
	CFLAGS_GCC_PSX += -I$(MAUG_ROOT)/api/sound/psx
endif

ifeq ("$$(FORCE_MVFS)","1")
	CFLAGS_GCC_PSX += -I$(MAUG_ROOT)/api/file/mvfs
else
	CFLAGS_GCC_PSX += -I$(MAUG_ROOT)/api/file/psn00b
endif

obj/psx/$(1).elf: \
$$(addprefix obj/psx/,$$(subst .c,.o,$$(C_FILES_PSX)))
	$$(CC_GPP_PSX) -o "$$@" \
		$$(LDFLAGS_GCC_PSX_FRONT) $$^ $$(LDFLAGS_GCC_PSX_BACK) \
		$$(LDFLAGS_GCC_PSX_PSN)

obj/psx/%.o: %.c | $$(MAUG_DEPS)
	$$(MD) $$(dir $$@)
	$$(CC_GCC_PSX) -c $$< -o $$@ \
		$$(INCLUDES_GCC) $$(DEFINES_GCC) $$(GLOBAL_DEFINES) \
		$$(CFLAGS_GCC_PSX) $$(CFLAGS_OPT_GCC_PSX) $$(CFLAGS_GCC_PSX_PSN) \
		$$(CFLAGS_OPT_GCC)

$(1)psx.exe: obj/psx/$(1).elf
	elf2x -q $$< $$@

obj/psx/$(1)psx.map: obj/psx/$(1).elf
	/usr/mipsel-none-elf/bin/mipsel-none-elf-nm -f posix -l -n $$< >$$@

obj/psx/system.cnf:
	# Generate the system.cnf.
	echo BOOT=cdrom:\$(1)psx.exe\;1 >> $$@
	echo TCB=4 >> $$@
	echo EVENT=10 >> $$@
	echo STACK=801FFFF0 >> $$@

obj/psx/iso.xml: obj/psx/system.cnf obj/psx/isoxml
	# Generate the iso.xml.
	echo "<?xml version=\"1.0\" encoding=\"utf-8\"?>" > $$@
	echo "<iso_project>" >> $$@
	echo "	<track type=\"data\">" >> $$@
	echo "		<identifiers" >> $$@
	echo "			system			=\"PLAYSTATION\"" >> $$@
	echo "			volume			=\"`echo $(1) | tr [:lower:] [:upper:]`\"" >> $$@
	echo "			volume_set		=\"`echo $(1) | tr [:lower:] [:upper:]`\"" >> $$@
	echo "			publisher		=\"MAUG\"" >> $$@
	echo "			data_preparer	=\"PSN00BSDK $$$${PSN00BSDK_VERSION}\"" >> $$@
	echo "			application		=\"PLAYSTATION\"" >> $$@
	echo "			copyright		=\"README.TXT;1\"" >> $$@
	echo "		/>" >> $$@
	echo "		<directory_tree>" >> $$@
	echo "			<file name=\"SYSTEM.CNF\"		type=\"data\" source=\"system.cnf\" />" >> $$@
	echo "			<file name=\"`echo $(1)psx.exe | tr [:lower:] [:upper:]`\"		type=\"data\" source=\"../../$(1)psx.exe\" />" >> $$@
	./obj/psx/isoxml $(2) >> $$@
	echo "" >> $$@
	echo "			<dummy sectors=\"1024\"/>" >> $$@
	echo "		</directory_tree>" >> $$@
	echo "	</track>" >> $$@
	echo "</iso_project>" >> $$@

obj/psx/isoxml: $(MAUG_ROOT)/tools/isoxml.c
	gcc -o $$@ $$<

$(1)psx.cue $(1)psx.bin: \
$(1)psx.exe obj/psx/$(1)psx.map obj/psx/iso.xml
	mkpsxiso -y -o $(1)psx.bin -c $(1)psx.cue obj/psx/iso.xml

CLEAN_TARGETS += $(1)psx.bin $(1)psx.cue $(1)psx.exe

endef

