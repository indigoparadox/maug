
# vim: ft=make noexpandtab

CC_CECL_SH3 := wine $(WCE20DEV)/BIN/SHCL.EXE

CC_CECL_MIPS := wine $(WCE20DEV)/BIN/CLMIPS.EXE

LD_CECL := wine $(WCE20DEV)/BIN/LINK.EXE

OBJDIR_CECL_WINCE_SH3 := obj/cecl-wince-sh3

OBJDIR_CECL_WINCE_MIPS := obj/cecl-wince-mips

CFLAGS_CECL_WINCE += /nologo /GX- /D "_WIN32_WCE" /D "UNICODE" /YX /I "$(WCE20DEV)/INCLUDE" /I "$(MAUG_ROOT)/api/win" /D "RETROFLAT_OS_WIN" /D "RETROFLAT_API_WINCE" /D "MAUG_ANCIENT_C" /D "RETROFLAT_NO_CLI" /D "MAUG_NO_STAT" /D "RETROFLAT_NO_STRING" /D "MAUG_NO_STDLIB" /D "MAUG_NO_FILE" $(INCLUDES_VS) /YX /c

LDFLAGS_CECL_WINCE += commctrl.lib coredll.lib /nologo /subsystem:windowsce

define TGT_CECL_WINCE_SH3

CFLAGS_CECL_WINCE_SH3 += /D "SH3" /D "_SH3_"

# /D "RETROFLAT_XPM" /D "MVFS_ENABLED" /I "obj/mvfs" /I "obj/retroxpm" 
CFLAGS_OPT_SH3 := $$(subst -DRETROFLAT_XPM -I$$(RETROXPM_H_DIR),/D "RETROFLAT_XPM" /I "$$(RETROXPM_H_DIR)",$$(CFLAGS_OPT_GCC))
CFLAGS_OPT_SH3 := $$(subst -D$$(MVFS_DEFINE) -I$$(MVFS_H_DIR),/D "$$(MVFS_DEFINE)" /I "$$(MVFS_H_DIR)",$$(CFLAGS_OPT_SH3))

LDFLAGS_CECL_WINCE_SH3 += /LIBPATH:$(WCE20DEV)/LIB/wce200/HPC/wcesh/ /machine:SH3

$(1)c3.exe: \
$$(addprefix $$(OBJDIR_CECL_WINCE_SH3)/,$$(subst .c,.o,$$(C_FILES))) | \
$$(RES_CECL_WINCE_SH3)
	$$(LD_CECL) \
		$$(LDFLAGS_CECL_WINCE_SH3) \
		$$(LDFLAGS_CECL) $$(LDFLAGS_CECL_WINCE) \
		$$(RES_CECL_WINCE_SH3_DEP) \
		$$^ /OUT:$$@

$$(OBJDIR_CECL_WINCE_SH3)/%.o: %.c | $$(MAUG_DEPS)
	$$(MD) "$$(dir $$@)"
	$$(CC_CECL_SH3) \
		$$(CFLAGS_CECL_WINCE_SH3) \
		$$(CFLAGS_CECL) $$(CFLAGS_CECL_WINCE) \
		$$(CFLAGS_OPT_SH3) \
		/Fo$$@ $$(<)

CLEAN_TARGETS += $(1)c3.exe

endef

# ---

define TGT_CECL_WINCE_MIPS

CFLAGS_CECL_WINCE_MIPS += /D "MIPS" /D "_MIPS_" /QMRWCE

# /D "RETROFLAT_XPM" /D "MVFS_ENABLED" /I "obj/mvfs" /I "obj/retroxpm" 
CFLAGS_OPT_MIPS := $$(subst -DRETROFLAT_XPM -I$$(RETROXPM_H_DIR),/D "RETROFLAT_XPM" /I "$$(RETROXPM_H_DIR)",$$(CFLAGS_OPT_GCC))
CFLAGS_OPT_MIPS := $$(subst -D$$(MVFS_DEFINE) -I$$(MVFS_H_DIR),/D "$$(MVFS_DEFINE)" /I "$$(MVFS_H_DIR)",$$(CFLAGS_OPT_MIPS))

LDFLAGS_CECL_WINCE_MIPS += /LIBPATH:$(WCE20DEV)/LIB/wce200/HPC/wcemips/ /machine:MIPS

$(1)cm.exe: \
$$(addprefix $$(OBJDIR_CECL_WINCE_MIPS)/,$$(subst .c,.o,$$(C_FILES))) | \
$$(RES_CECL_WINCE_MIPS)
	$$(LD_CECL) \
		$$(LDFLAGS_CECL_WINCE_MIPS) \
		$$(LDFLAGS_CECL) $$(LDFLAGS_CECL_WINCE) \
		$$(RES_CECL_WINCE_MIPS_DEP) \
		$$^ /OUT:$$@

$$(OBJDIR_CECL_WINCE_MIPS)/%.o: %.c | $$(MAUG_DEPS)
	$$(MD) "$$(dir $$@)"
	$$(CC_CECL_MIPS) \
		$$(CFLAGS_CECL_WINCE_MIPS) \
		$$(CFLAGS_CECL) $$(CFLAGS_CECL_WINCE) \
		$$(CFLAGS_OPT_MIPS) \
		/Fo$$@ $$(<)

CLEAN_TARGETS += $(1)cm.exe

endef

