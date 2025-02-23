
# vim: ft=make noexpandtab

CC_CECL_SH3 := wine $(WCE20DEV)/BIN/SHCL.EXE

CC_CECL_MIPS := wine $(WCE20DEV)/BIN/CLMIPS.EXE

CC_CECL_X86 := wine $(WCE20DEV)/BIN/CL.EXE

LD_CECL := wine $(WCE20DEV)/BIN/LINK.EXE

OBJDIR_CECL_WINCE_SH3 := obj/cecl-wince-sh3

OBJDIR_CECL_WINCE_MIPS := obj/cecl-wince-mips

OBJDIR_CECL_WINCE_X86 := obj/cecl-wince-x86

CFLAGS_CECL_WINCE += /nologo /GX- /D "_WIN32_WCE" /D "UNICODE" /I "$(WCE20DEV)/INCLUDE" /I "$(MAUG_ROOT)/api/win" /D "RETROFLAT_OS_WIN" /D "RETROFLAT_API_WINCE" /D "MAUG_ANCIENT_C" /D "RETROFLAT_NO_CLI" /D "RETROFLAT_NO_STRING" /D "MAUG_NO_STDLIB" /D "MAUG_WCHAR" /D "NO_RETROSND" $(INCLUDES_VS)

LDFLAGS_CECL_WINCE += commctrl.lib coredll.lib /nologo /subsystem:windowsce

# ---

define TGT_ICO_WINCE

# General utility macro to be called below to pick up the application icon
# (<appname>.bmp) *or* the default maug icon if no application icon present.

$(2)/win_ico.ico:
	$$(MD) $$(dir $$@)
	if [ -f "$$<.bmp" ]; then \
		convert "$$<.bmp" -transparent black "$$@"; \
	else \
		convert "$$(MAUG_ROOT)/maug16.bmp" -transparent black "$$@"; \
	fi

$(2)/$(1).rc: $(2)/win_ico.ico
	echo "$$(WIN_ICO_RES_ID)   ICON   \"$(2)/win_ico.ico\"" > $$@

endef

# ---

define TGT_CECL_WINCE_SH3

CFLAGS_CECL_WINCE_SH3 += /D "SH3" /D "_SH3_"

# /D "RETROFLAT_XPM" /D "MVFS_ENABLED" /I "obj/mvfs" /I "obj/retroxpm" 
CFLAGS_OPT_SH3 := $$(subst -DRETROFLAT_XPM -I$$(RETROXPM_H_DIR),/D "RETROFLAT_XPM" /I "$$(RETROXPM_H_DIR)",$$(CFLAGS_OPT_GCC))
CFLAGS_OPT_SH3 := $$(subst -D$$(MVFS_DEFINE) -I$$(MVFS_H_DIR),/D "$$(MVFS_DEFINE)" /I "$$(MVFS_H_DIR)",$$(CFLAGS_OPT_SH3))

LDFLAGS_CECL_WINCE_SH3 += /LIBPATH:$(WCE20DEV)/LIB/wce200/HPC/wcesh/ /machine:SH3

$(eval $(call TGT_ICO_WINCE,$(1),$$(OBJDIR_CECL_WINCE_SH3)))

$$(OBJDIR_CECL_WINCE_SH3)/$(1).res: $$(OBJDIR_CECL_WINCE_SH3)/$(1).rc
	wine $(WCE20DEV)/BIN/RC.EXE /l 0x409 /r /d "SH3" /d "_SH3_" /d "_WIN32_WCE" /d "UNICODE" /fo $$@ $$<
# /d "NDEBUG"

$(1)c3.exe: \
$$(addprefix $$(OBJDIR_CECL_WINCE_SH3)/,$$(subst .c,.o,$$(C_FILES))) | \
$$(OBJDIR_CECL_WINCE_SH3)/$(1).res
	$$(LD_CECL) \
		$$(LDFLAGS_CECL_WINCE_SH3) \
		$$(LDFLAGS_CECL) $$(LDFLAGS_CECL_WINCE) \
		$$(OBJDIR_CECL_WINCE_SH3)/$(1).res \
		$$^ /OUT:$$@

$$(OBJDIR_CECL_WINCE_SH3)/%.o: %.c | $$(MAUG_DEPS)
	$$(MD) "$$(dir $$@)"
	$$(CC_CECL_SH3) \
		$$(CFLAGS_CECL_WINCE_SH3) \
		$$(CFLAGS_CECL) $$(CFLAGS_CECL_WINCE) \
		$$(CFLAGS_OPT_SH3) \
		/c /Fo$$@ $$(<)

CLEAN_TARGETS += $(1)c3.exe

endef

# ---

define TGT_CECL_WINCE_MIPS

CFLAGS_CECL_WINCE_MIPS += /D "MIPS" /D "_MIPS_" /QMRWCE

# /D "RETROFLAT_XPM" /D "MVFS_ENABLED" /I "obj/mvfs" /I "obj/retroxpm" 
CFLAGS_OPT_MIPS := $$(subst -DRETROFLAT_XPM -I$$(RETROXPM_H_DIR),/D "RETROFLAT_XPM" /I "$$(RETROXPM_H_DIR)",$$(CFLAGS_OPT_GCC))
CFLAGS_OPT_MIPS := $$(subst -D$$(MVFS_DEFINE) -I$$(MVFS_H_DIR),/D "$$(MVFS_DEFINE)" /I "$$(MVFS_H_DIR)",$$(CFLAGS_OPT_MIPS))

LDFLAGS_CECL_WINCE_MIPS += /LIBPATH:$(WCE20DEV)/LIB/wce200/HPC/wcemips/ /machine:MIPS

$(eval $(call TGT_ICO_WINCE,$(1),$$(OBJDIR_CECL_WINCE_MIPS)))

$$(OBJDIR_CECL_WINCE_MIPS)/$(1).res: $$(OBJDIR_CECL_WINCE_MIPS)/$(1).rc
	wine $(WCE20DEV)/BIN/RC.EXE /l 0x409 /r /d "MIPS" /d "_MIPS_" /d "_WIN32_WCE" /d "UNICODE" /fo $$@ $$<

$(1)cm.exe: \
$$(addprefix $$(OBJDIR_CECL_WINCE_MIPS)/,$$(subst .c,.o,$$(C_FILES))) | \
$$(OBJDIR_CECL_WINCE_MIPS)/$(1).res
	$$(LD_CECL) \
		$$(LDFLAGS_CECL_WINCE_MIPS) \
		$$(LDFLAGS_CECL) $$(LDFLAGS_CECL_WINCE) \
		$$(OBJDIR_CECL_WINCE_MIPS)/$(1).res \
		$$^ /OUT:$$@

$$(OBJDIR_CECL_WINCE_MIPS)/%.o: %.c | $$(MAUG_DEPS)
	$$(MD) "$$(dir $$@)"
	$$(CC_CECL_MIPS) \
		$$(CFLAGS_CECL_WINCE_MIPS) \
		$$(CFLAGS_CECL) $$(CFLAGS_CECL_WINCE) \
		$$(CFLAGS_OPT_MIPS) \
		/c /Fo$$@ $$(<)

CLEAN_TARGETS += $(1)cm.exe

endef

# ---

define TGT_CECL_WINCE_X86

CFLAGS_CECL_WINCE_X86 += /D "_WINDOWS" /D "UNDER_NT" /D "_WIN32_WCE_EMULATION" /D "_X86_" /D "_UNICODE" /D "WIN32"

# /D "RETROFLAT_XPM" /D "MVFS_ENABLED" /I "obj/mvfs" /I "obj/retroxpm" 
CFLAGS_OPT_X86 := $$(subst -DRETROFLAT_XPM -I$$(RETROXPM_H_DIR),/D "RETROFLAT_XPM" /I "$$(RETROXPM_H_DIR)",$$(CFLAGS_OPT_GCC))
CFLAGS_OPT_X86 := $$(subst -D$$(MVFS_DEFINE) -I$$(MVFS_H_DIR),/D "$$(MVFS_DEFINE)" /I "$$(MVFS_H_DIR)",$$(CFLAGS_OPT_X86))

LDFLAGS_CECL_WINCE_X86 := /LIBPATH:$(WCE20DEV)/LIB/wce200/HPC/x86em/ /LIBPATH:$(WCE20DEV)/LIB/vc5/ comctl32.lib coredll.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /machine:I386

# TODO: Icon support.

$(1)cx.exe: \
$$(addprefix $$(OBJDIR_CECL_WINCE_X86)/,$$(subst .c,.o,$$(C_FILES))) | \
$$(RES_CECL_WINCE_X86)
	$$(LD_CECL) \
		$$(LDFLAGS_CECL_WINCE_X86) \
		$$(LDFLAGS_CECL) $$(RES_CECL_WINCE_X86_DEP) \
		$$^ /OUT:$$@ /windowsce:emulation

$$(OBJDIR_CECL_WINCE_X86)/%.o: %.c | $$(MAUG_DEPS)
	$$(MD) "$$(dir $$@)"
	$$(CC_CECL_X86) \
		$$(CFLAGS_CECL_WINCE_X86) \
		$$(CFLAGS_CECL) $$(CFLAGS_CECL_WINCE) \
		$$(CFLAGS_OPT_X86) \
		/c /Fo$$@ $$(<)

CLEAN_TARGETS += $(1)cx.exe

endef

