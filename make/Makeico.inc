
# vim: ft=make noexpandtab

OBJDIR_ICO=obj/ico

# ---

define TGTWINICO

ifndef $$(WIN_ICO_RES_ID)
	WIN_ICO_RES_ID=1000
endif

# Create icon and resource file.

# Watcom-Win-Win32

# Don't use MAUG_DEPS since these have to be passed directly to the compiler.
RES_WATCOM_WIN_WIN32_DEP=$$(OBJDIR_WATCOM_WIN_WIN32)/$(1).res
RES_WATCOM_WIN_WIN32=resource {$$(RES_WATCOM_WIN_WIN32_DEP)}

$$(OBJDIR_WATCOM_WIN_WIN32)/$(1).ico: $(1).bmp
	convert "$$<" "$$@"

$$(OBJDIR_WATCOM_WIN_WIN32)/$(1).rc: $$(OBJDIR_WATCOM_WIN_WIN32)/$(1).ico
	echo "$$(WIN_ICO_RES_ID)   ICON   \"$$(OBJDIR_WATCOM_WIN_WIN32)/$(1).ico\"" > $$@

$$(OBJDIR_WATCOM_WIN_WIN32)/$(1).res: $$(OBJDIR_WATCOM_WIN_WIN32)/$(1).rc
	wrc -r $$< -bt=nt -fo=$$@

# Watcom-Win-Win16

$$(OBJDIR_WATCOM_WIN_WIN16)/$(1).ico: $(1).bmp
	convert "$$<" "$$@"

$$(OBJDIR_WATCOM_WIN_WIN16)/$(1).rc: $$(OBJDIR_WATCOM_WIN_WIN16)/$(1).ico
	echo "$$(WIN_ICO_RES_ID)   ICON   \"$$(OBJDIR_WATCOM_WIN_WIN16)/$(1).ico\"" > $$@

# Watcom386-Win-Win16

$$(OBJDIR_WATCOM386_WIN_WIN16)/$(1).ico: $(1).bmp
	convert "$$<" "$$@"

$$(OBJDIR_WATCOM386_WIN_WIN16)/$(1).rc: $$(OBJDIR_WATCOM386_WIN_WIN16)/$(1).ico
	echo "$$(WIN_ICO_RES_ID)   ICON   \"$$(OBJDIR_WATCOM386_WIN_WIN16)/$(1).ico\"" > $$@

# GCC-Win-Win32

RES_GCC_WIN_WIN32_DEP=$$(OBJDIR_GCC_WIN_WIN32)/$(1).res

$$(OBJDIR_GCC_WIN_WIN32)/$(1).ico: $(1).bmp
	convert "$$<" "$$@"

$$(OBJDIR_GCC_WIN_WIN32)/$(1).rc: $$(OBJDIR_GCC_WIN_WIN32)/$(1).ico
	echo "$$(WIN_ICO_RES_ID)   ICON   \"$$(OBJDIR_GCC_WIN_WIN32)/$(1).ico\"" > $$@

$$(OBJDIR_GCC_WIN_WIN32)/$(1).res: $$(OBJDIR_GCC_WIN_WIN32)/$(1).rc
	$(WINDRES_GCC_MINGW) "$$<" -O coff -o "$$@"

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

# ---

define TGTSDLICO

$$(OBJDIR_ICO)/sdl_ico.bmp: $(1).bmp
	$$(MD) "$$(dir $$@)"
	cp "$(1).bmp" "$$@"

$$(OBJDIR_ICO)/sdl_ico.h: $$(OBJDIR_ICO)/sdl_ico.bmp
	xxd -i $$< $$@

MAUG_DEPS += $$(OBJDIR_ICO)/sdl_ico.h

# Enable icon inclusion and define guard.
DEFINES_GCC += -DRETROFLAT_SDL_ICO
INCLUDES_GCC += -I$$(OBJDIR_ICO)
CFLAGS_GCC += -DRETROFLAT_SDL_ICO -I$$(OBJDIR_ICO)

endef

