
# vim: ft=make noexpandtab

OBJDIR_ICO=obj/ico

# ---

define TGTWINICO

ifndef $$(WIN_ICO_RES_ID)
	WIN_ICO_RES_ID=1000
endif

# ---
# Create icon and resource file.
# ---

# CECL-WinCE-SH3

# Don't use MAUG_DEPS since these have to be passed directly to the compiler.
RES_CECL_WINCE_SH3_DEP=$$(OBJDIR_CECL_WINCE_SH3)/$(1).res
RES_CECL_WINCE_SH3=$$(RES_CECL_WINCE_SH3_DEP)

$$(OBJDIR_CECL_WINCE_SH3)/$(1).ico: $(1).bmp
	convert "$$<" "$$@"

$$(OBJDIR_CECL_WINCE_SH3)/$(1).rc: $$(OBJDIR_CECL_WINCE_SH3)/$(1).ico
	echo "$$(WIN_ICO_RES_ID)   ICON   \"$$(OBJDIR_CECL_WINCE_SH3)/$(1).ico\"" > $$@

$$(OBJDIR_CECL_WINCE_SH3)/$(1).res: $$(OBJDIR_CECL_WINCE_SH3)/$(1).rc
	wine $(WCE20DEV)/BIN/RC.EXE /l 0x409 /r /d "SH3" /d "_SH3_" /d "_WIN32_WCE" /d "UNICODE" /fo $$@ $$<
# /d "NDEBUG"

# ---

# CECL-WinCE-MIPS

# Don't use MAUG_DEPS since these have to be passed directly to the compiler.
RES_CECL_WINCE_MIPS_DEP=$$(OBJDIR_CECL_WINCE_MIPS)/$(1).res
RES_CECL_WINCE_MIPS=$$(RES_CECL_WINCE_MIPS_DEP)

$$(OBJDIR_CECL_WINCE_MIPS)/$(1).ico: $(1).bmp
	convert "$$<" "$$@"

$$(OBJDIR_CECL_WINCE_MIPS)/$(1).rc: $$(OBJDIR_CECL_WINCE_MIPS)/$(1).ico
	echo "$$(WIN_ICO_RES_ID)   ICON   \"$$(OBJDIR_CECL_WINCE_MIPS)/$(1).ico\"" > $$@

$$(OBJDIR_CECL_WINCE_MIPS)/$(1).res: $$(OBJDIR_CECL_WINCE_MIPS)/$(1).rc
	wine $(WCE20DEV)/BIN/RC.EXE /l 0x409 /r /d "MIPS" /d "_MIPS_" /d "_WIN32_WCE" /d "UNICODE" /fo $$@ $$<
# /d "NDEBUG"

# ---

# Watcom-Win-Win32

# Don't use MAUG_DEPS since these have to be passed directly to the compiler.
RES_WATCOM386_WIN_WIN32_DEP=$$(OBJDIR_WATCOM386_WIN_WIN32)/$(1).res
RES_WATCOM386_WIN_WIN32=resource {$$(RES_WATCOM386_WIN_WIN32_DEP)}

$$(OBJDIR_WATCOM386_WIN_WIN32)/$(1).ico: $(1).bmp
	convert "$$<" "$$@"

$$(OBJDIR_WATCOM386_WIN_WIN32)/$(1).rc: $$(OBJDIR_WATCOM386_WIN_WIN32)/$(1).ico
	echo "$$(WIN_ICO_RES_ID)   ICON   \"$$(OBJDIR_WATCOM386_WIN_WIN32)/$(1).ico\"" > $$@

$$(OBJDIR_WATCOM386_WIN_WIN32)/$(1).res: $$(OBJDIR_WATCOM386_WIN_WIN32)/$(1).rc
	wrc -r $$< -bt=nt -fo=$$@

# ---

# Watcom-Win-GLUT

# Don't use MAUG_DEPS since these have to be passed directly to the compiler.
RES_WATCOM386_WIN_GLUT_DEP=$$(OBJDIR_WATCOM386_WIN_GLUT)/$(1).res
RES_WATCOM386_WIN_GLUT=resource {$$(RES_WATCOM386_WIN_GLUT_DEP)}

$$(OBJDIR_WATCOM386_WIN_GLUT)/$(1).ico: $(1).bmp
	convert "$$<" "$$@"

$$(OBJDIR_WATCOM386_WIN_GLUT)/$(1).rc: $$(OBJDIR_WATCOM386_WIN_GLUT)/$(1).ico
	echo "$$(WIN_ICO_RES_ID)   ICON   \"$$(OBJDIR_WATCOM386_WIN_GLUT)/$(1).ico\"" > $$@
	echo "GLUT_ICON   ICON   \"$$(OBJDIR_WATCOM386_WIN_GLUT)/$(1).ico\"" >> $$@

$$(OBJDIR_WATCOM386_WIN_GLUT)/$(1).res: $$(OBJDIR_WATCOM386_WIN_GLUT)/$(1).rc
	wrc -r $$< -bt=nt -fo=$$@

# ---

# Watcom-Win-SDL

# Don't use MAUG_DEPS since these have to be passed directly to the compiler.
RES_WATCOM386_WIN_SDL_DEP=$$(OBJDIR_WATCOM386_WIN_SDL)/$(1).res
RES_WATCOM386_WIN_SDL=resource {$$(RES_WATCOM386_WIN_SDL_DEP)}

$$(OBJDIR_WATCOM386_WIN_SDL)/$(1).ico: $(1).bmp
	convert "$$<" "$$@"

$$(OBJDIR_WATCOM386_WIN_SDL)/$(1).rc: $$(OBJDIR_WATCOM386_WIN_SDL)/$(1).ico
	echo "$$(WIN_ICO_RES_ID)   ICON   \"$$(OBJDIR_WATCOM386_WIN_SDL)/$(1).ico\"" > $$@

$$(OBJDIR_WATCOM386_WIN_SDL)/$(1).res: $$(OBJDIR_WATCOM386_WIN_SDL)/$(1).rc
	wrc -r $$< -bt=nt -fo=$$@

# Watcom-Win-Win16

RES_WATCOM_WIN_WIN16_DEP=$$(OBJDIR_WATCOM_WIN_WIN16)/$(1).res
RES_WATCOM_WIN_WIN16=option resource=$$(RES_WATCOM_WIN_WIN16_DEP)

$$(OBJDIR_WATCOM_WIN_WIN16)/$(1).ico: $(1).bmp
	convert "$$<" "$$@"

$$(OBJDIR_WATCOM_WIN_WIN16)/$(1).rc: $$(OBJDIR_WATCOM_WIN_WIN16)/$(1).ico
	echo "$$(WIN_ICO_RES_ID)   ICON   \"$$(OBJDIR_WATCOM_WIN_WIN16)/$(1).ico\"" > $$@

$$(OBJDIR_WATCOM_WIN_WIN16)/$(1).res: $$(OBJDIR_WATCOM_WIN_WIN16)/$(1).rc
	wrc -r $$< -bt=windows -fo=$$@

# ---

# Watcom386-Win-Win16

# This just generates an rc file which wbind will then process when adding
# the Win386 runtime.

RES_WATCOM386_WIN_WIN16_DEP=$$(OBJDIR_WATCOM386_WIN_WIN16)/$(1).rc
RES_WATCOM386_WIN_WIN16=$$(RES_WATCOM386_WIN_WIN16_DEP)

$$(OBJDIR_WATCOM386_WIN_WIN16)/$(1).ico: $(1).bmp
	convert "$$<" "$$@"

$$(OBJDIR_WATCOM386_WIN_WIN16)/$(1).rc: $$(OBJDIR_WATCOM386_WIN_WIN16)/$(1).ico
	echo "$$(WIN_ICO_RES_ID)   ICON   \"$$(OBJDIR_WATCOM386_WIN_WIN16)/$(1).ico\"" > $$@

# ---

# GCC-Win-Win32

RES_GCC_WIN_WIN32_DEP=$$(OBJDIR_GCC_WIN_WIN32)/$(1).res

$$(OBJDIR_GCC_WIN_WIN32)/$(1).ico: $(1).bmp
	convert "$$<" "$$@"

$$(OBJDIR_GCC_WIN_WIN32)/$(1).rc: $$(OBJDIR_GCC_WIN_WIN32)/$(1).ico
	echo "$$(WIN_ICO_RES_ID)   ICON   \"$$(OBJDIR_GCC_WIN_WIN32)/$(1).ico\"" > $$@

$$(OBJDIR_GCC_WIN_WIN32)/$(1).res: $$(OBJDIR_GCC_WIN_WIN32)/$(1).rc
	$(WINDRES_GCC_MINGW) "$$<" -O coff -o "$$@"

# ---

# GCC64-Win-Win32

RES_GCC64_WIN_WIN32_DEP=$$(OBJDIR_GCC64_WIN_WIN32)/$(1).res

$$(OBJDIR_GCC64_WIN_WIN32)/$(1).ico: $(1).bmp
	convert "$$<" "$$@"

$$(OBJDIR_GCC64_WIN_WIN32)/$(1).rc: $$(OBJDIR_GCC64_WIN_WIN32)/$(1).ico
	echo "$$(WIN_ICO_RES_ID)   ICON   \"$$(OBJDIR_GCC64_WIN_WIN32)/$(1).ico\"" > $$@

$$(OBJDIR_GCC64_WIN_WIN32)/$(1).res: $$(OBJDIR_GCC64_WIN_WIN32)/$(1).rc
	$(WINDRES_GCC64_MINGW) "$$<" -O coff -o "$$@"

CFLAGS_GCC_WIN += -DRETROFLAT_ICO_RES_ID=$$(WIN_ICO_RES_ID)
CFLAGS_WATCOM_WIN += -DRETROFLAT_ICO_RES_ID=$$(WIN_ICO_RES_ID)

endef

