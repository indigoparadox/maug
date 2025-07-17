
# vim: ft=make noexpandtab

# Setup default CFLAGS/LDFLAGS for all Windows targets.
ifeq ("$(CFLAGS_WATCOM_WIN)","")
	CFLAGS_WATCOM_WIN := -DRETROFLAT_OS_WIN -DRETROSND_API_WINMM -DRETROSND_ARGS
endif
ifeq ("$(LDFLAGS_WATCOM_WIN)","")
	LDFLAGS_WATCOM_WIN := libr mmsystem
endif

ifeq ("$(CFLAGS_WATCOM386_WIN)","")
	CFLAGS_WATCOM386_WIN := -DRETROFLAT_OS_WIN  -DRETROSND_API_WINMM -DRETROSND_ARGS
endif
ifeq ("$(LDFLAGS_WATCOM386_WIN)","")
	LDFLAGS_WATCOM386_WIN := libr winmm
endif

ifeq ("$(CFLAGS_GCC_WIN)","")
	CFLAGS_GCC_WIN := -march=i486 -DRETROFLAT_OS_WIN -DRETROSND_API_WINMM -DRETROSND_ARGS
endif
ifeq ("$(LDFLAGS_GCC_WIN)","")
	LDFLAGS_GCC_WIN := -mwindows -lgdi32 -lwinmm
endif

ifeq ("$(CFLAGS_GCC64_WIN)","")
	CFLAGS_GCC64_WIN := -DRETROFLAT_OS_WIN  -DRETROSND_API_WINMM -DRETROSND_ARGS
endif
ifeq ("$(LDFLAGS_GCC64_WIN)","")
	LDFLAGS_GCC64_WIN := -mwindows -lgdi32 -lwinmm
endif

ifeq ("$(BUILD)","RELEASE")
	CFLAGS_GCC_WIN += -O2
	CFLAGS_GCC64_WIN += -O2
	CFLAGS_WATCOM386 += -ol+ -ok -oi -oh -or
ifeq ("$(RETROFLAT_OPENGL)","1")
	CFLAGS_WATCOM386 += -ob -on
endif
endif

ifneq ("$(INLINE_SZ)","")
	CFLAGS_WATCOM386 += -oe=$(INLINE_SZ)
endif

ifeq ("$(CC_GCC_MINGW)","")
	CC_GCC_MINGW = i686-w64-mingw32-gcc
endif
ifeq ("$(LD_GCC_MINGW)","")
	LD_GCC_MINGW = i686-w64-mingw32-gcc
endif
ifeq ("$(WINDRES_GCC_MINGW)","")
	WINDRES_GCC_MINGW = i686-w64-mingw32-windres
endif

ifeq ("$(CC_GCC_MINGW64)","")
	CC_GCC_MINGW64 = x86_64-w64-mingw32-gcc
endif
ifeq ("$(LD_GCC_MINGW64)","")
	LD_GCC_MINGW64 = x86_64-w64-mingw32-gcc
endif
ifeq ("$(WINDRES_GCC64_MINGW)","")
	WINDRES_GCC64_MINGW = x86_64-w64-mingw32-windres
endif

ifneq ("$(BUILD)","RELEASE")
ifneq ("$(WIN_FORCE_CONSOLE_LOG)","1")
	CFLAGS_WATCOM_WIN += -DLOG_TO_FILE -DLOG_FILE_NAME=\"out.log\"
	CFLAGS_WATCOM386_WIN += -DLOG_TO_FILE -DLOG_FILE_NAME=\"out.log\"
	CFLAGS_GCC_WIN += -DLOG_TO_FILE -DLOG_FILE_NAME=\"out.log\"
	CFLAGS_GCC64_WIN += -DLOG_TO_FILE -DLOG_FILE_NAME=\"out.log\"
endif
endif

# Common Variables

# TODO: Right now, these must be specified before inclusion and apply to all
# Windows targets... maybe make them per-target instead?

ifeq ("$(SCREENSAVER)","1")
	CFLAGS_WATCOM_WIN += -DRETROFLAT_SCREENSAVER
	CFLAGS_WATCOM386_WIN += -DRETROFLAT_SCREENSAVER
	CFLAGS_GCC_WIN += -DRETROFLAT_SCREENSAVER
	CFLAGS_GCC64_WIN += -DRETROFLAT_SCREENSAVER
endif

ifeq ("$(RETROFLAT_OPENGL)","1")
	LDFLAGS_WATCOM386_WIN += libr opengl32
	LDFLAGS_GCC_WIN += -lopengl32
	LDFLAGS_GCC64_WIN += -lopengl32
endif

ifeq ("$(WING)","1")
	CFLAGS_WATCOM_WIN += -DRETROFLAT_WING
	CFLAGS_WATCOM386_WIN += -DRETROFLAT_WING
endif

ifeq ("$(RETROFLAT_OPENGL)","1")
CFLAGS_WATCOM386_WIN += -I$(MAUG_ROOT)/api/retro3d/opengl
CFLAGS_GCC_WIN += -I$(MAUG_ROOT)/api/retro3d/opengl
CFLAGS_GCC64_WIN += -I$(MAUG_ROOT)/api/retro3d/opengl
endif

CFLAGS_GCC_WIN_WIN32 := -DRETROFLAT_API_WIN32
CFLAGS_GCC64_WIN_WIN32 := -DRETROFLAT_API_WIN32

OBJDIR_WATCOM386_WIN_WIN32=obj/watcom-win-win32
OBJDIR_WATCOM386_WIN_GLUT=obj/watcom-win-glut
OBJDIR_WATCOM386_WIN_SDL=obj/watcom-win-sdl
OBJDIR_WATCOM_WIN_WIN16=obj/watcom-win-win16
OBJDIR_WATCOM386_WIN_WIN16=obj/watcom386-win-win16
OBJDIR_GCC_WIN_WIN32=obj/gcc-win-win32
OBJDIR_GCC64_WIN_WIN32=obj/gcc64-win-win32

ifndef $(WIN_ICO_RES_ID)
	WIN_ICO_RES_ID=1000
	CFLAGS_WATCOM_WIN += -DRETROFLAT_ICO_RES_ID=$(WIN_ICO_RES_ID)
	CFLAGS_WATCOM386_WIN += -DRETROFLAT_ICO_RES_ID=$(WIN_ICO_RES_ID)
	CFLAGS_GCC_WIN += -DRETROFLAT_ICO_RES_ID=$(WIN_ICO_RES_ID)
	CFLAGS_GCC64_WIN += -DRETROFLAT_ICO_RES_ID=$(WIN_ICO_RES_ID)
endif

# ---

define TGT_ICO_WIN

# General utility macro to be called below to pick up the application icon
# (<appname>.bmp) *or* the default maug icon if no application icon present.

# TODO: Look for <appname>16.bmp, <appname>32.bmp to make multi-size icons.

$(2)/win_ico.ico:
	$$(MD) $$(dir $$@)
	if [ -f "$(1).bmp" ]; then \
		convert "$(1).bmp" -transparent black "$$@"; \
	else \
		convert "$$(MAUG_ROOT)/maug16.bmp" -transparent black "$$@"; \
	fi

$(2)/$(1).rc: $(2)/win_ico.ico
	echo "$$(WIN_ICO_RES_ID)   ICON   \"$(2)/win_ico.ico\"" > $$@

endef

# ---

define TGT_WATCOM386_WIN_WIN32

CFLAGS_WATCOM386_WIN_WIN32 += \
	-mf -bt=nt -i=$(WATCOM)/h/nt -DRETROFLAT_API_WIN32 \
	-i=$(MAUG_ROOT)/api/retro2d/win -i=$(MAUG_ROOT)/api/input/win \
	-i=$(MAUG_ROOT)/api/file/win -i=$(MAUG_ROOT)/api/log/unix \
	-i=$(MAUG_ROOT)/api/font/win

ifeq ("$$(RETROFLAT_OPENGL)","1")
	# OpenGL stuff is pentium-optimized.
	CFLAGS_WATCOM386_WIN_WIN32 += -5r -fp5
else
	# Non-OpenGL can be on 386.
	CFLAGS_WATCOM386_WIN_WIN32 += -3r -fp3
endif

LDFLAGS_WATCOM386_WIN_WIN32 += system nt_win

ifeq ("$$(RETROFLAT_VDP)","1")
	CFLAGS_WATCOM386_WIN_WIN32 += -DRETROFLAT_VDP
endif

ifeq ("$$(RETROFLAT_SOUND)","1")
	CFLAGS_WATCOM386_WIN_WIN32 += -i=$(MAUG_ROOT)/api/sound/win
endif

$(eval $(call TGT_ICO_WIN,$(1),$$(OBJDIR_WATCOM386_WIN_WIN32)))

$$(OBJDIR_WATCOM386_WIN_WIN32)/$(1).res: $$(OBJDIR_WATCOM386_WIN_WIN32)/$(1).rc
	wrc -r $$< -bt=nt -fo=$$@

$(1)nt.exe: \
$$(addprefix $$(OBJDIR_WATCOM386_WIN_WIN32)/,$$(subst .c,.o,$$(C_FILES))) | \
$$(OBJDIR_WATCOM386_WIN_WIN32)/$(1).res
	$$(LD_WATCOM386) name $$@ \
		$$(LDFLAGS_WATCOM386) $$(LDFLAGS_WATCOM386_WIN) \
		$$(LDFLAGS_WATCOM386_WIN_WIN32) fil {$$^} \
		resource $$(OBJDIR_WATCOM386_WIN_WIN32)/$(1).res

$$(OBJDIR_WATCOM386_WIN_WIN32)/%.o: %.c | $$(MAUG_DEPS)
	$$(MD) "$$(dir $$@)"
	INCLUDE=$(WATCOM)/h $$(CC_WATCOM386) \
		$$(CFLAGS_WATCOM386) $$(CFLAGS_WATCOM386_WIN) \
		$$(CFLAGS_WATCOM386_WIN_WIN32) $$(CFLAGS_OPT_WATCOM) \
		-fo=$$@ $$(<:%.c=%)

CLEAN_TARGETS += $(1)nt.exe

endef

# Legacy
TGTWINNT=$(TGT_WATCOM386_WIN_WIN32)

# ---

define TGT_WATCOM386_WIN_GLUT

# GLUT requires stack calling (-5s) and is always on Pentium because OpenGL.
CFLAGS_WATCOM386_WIN_GLUT := \
	-mf -5s -fp5 -bt=nt -i=$(WATCOM)/h/nt -DRETROFLAT_API_GLUT \
	-i=$(MAUG_ROOT)/api/retro2d/glut -i=$(MAUG_ROOT)/api/input/glut \
	-i=$(MAUG_ROOT)/api/file/win -i=$(MAUG_ROOT)/api/log/unix \
	-i=$(MAUG_ROOT)/api/font/soft

LDFLAGS_WATCOM386_WIN_GLUT := system nt_win libr glut32

ifeq ("$$(RETROFLAT_SOUND)","1")
	CFLAGS_WATCOM386_WIN_GLUT += -i=$(MAUG_ROOT)/api/sound/null
endif

$(eval $(call TGT_ICO_WIN,$(1),$$(OBJDIR_WATCOM386_WIN_GLUT)))

$$(OBJDIR_WATCOM386_WIN_GLUT)/$(1).res: $$(OBJDIR_WATCOM386_WIN_GLUT)/$(1).rc
	wrc -r $$< -bt=nt -fo=$$@

$(1)ntgl.exe: \
$$(addprefix $$(OBJDIR_WATCOM386_WIN_GLUT)/,$$(subst .c,.o,$$(C_FILES))) | \
$$(OBJDIR_WATCOM386_WIN_GLUT)/$(1).res
	$$(LD_WATCOM386) name $$@ \
		$$(LDFLAGS_WATCOM386) $$(LDFLAGS_WATCOM386_WIN) \
		$$(LDFLAGS_WATCOM386_WIN_GLUT) fil {$$^} \
		resource $$(OBJDIR_WATCOM386_WIN_GLUT)/$(1).res

$$(OBJDIR_WATCOM386_WIN_GLUT)/%.o: %.c | $$(MAUG_DEPS)
	$$(MD) "$$(dir $$@)"
	INCLUDE=$(WATCOM)/h $$(CC_WATCOM386) \
		$$(CFLAGS_WATCOM386) $$(CFLAGS_WATCOM386_WIN) \
		$$(CFLAGS_WATCOM386_WIN_GLUT) $$(CFLAGS_OPT_WATCOM) \
		-fo=$$@ $$(<:%.c=%)

CLEAN_TARGETS += $(1)ntgl.exe

endef

# Legacy
TGTWINGL=$(TGT_WATCOM386_WIN_GLUT)

# ---

define TGT_WATCOM386_WIN_SDL

# SDL is always on Pentium.
CFLAGS_WATCOM386_WIN_SDL += \
	-mf -5s -ei -fp5 -dSDL_main=main -bt=nt -i=$(MAUG_ROOT)/api/retro2d/sdl \
	-i=$(MAUG_ROOT)/api/input/sdl -i=$(MAUG_ROOT)/api/log/unix \
	-i=$(WATCOM)/h/nt -i=$(WATCOM)/h/nt/SDL -DRETROFLAT_API_SDL1 \
	-i=$(MAUG_ROOT)/api/file/win -i=$(MAUG_ROOT)/api/font/soft \
	-DNO_RETROSND

LDFLAGS_WATCOM386_WIN_SDL += system nt_win libr {SDL SDLmain}

ifeq ("$$(RETROFLAT_SOUND)","1")
	CFLAGS_WATCOM386_WIN_SDL += -i=$(MAUG_ROOT)/api/sound/sdl
endif

$(eval $(call TGT_ICO_WIN,$(1),$$(OBJDIR_WATCOM386_WIN_SDL)))

# TODO: Import icon for use in SDL window.

$$(OBJDIR_WATCOM386_WIN_SDL)/$(1).res: $$(OBJDIR_WATCOM386_WIN_SDL)/$(1).rc
	wrc -r $$< -bt=nt -fo=$$@

$(1)nts.exe: \
$$(addprefix $$(OBJDIR_WATCOM386_WIN_SDL)/,$$(subst .c,.o,$$(C_FILES))) | \
$$(OBJDIR_WATCOM386_WIN_SDL)/$(1).res
	$$(LD_WATCOM386) name $$@ \
		$$(LDFLAGS_WATCOM386) $$(LDFLAGS_WATCOM386_WIN) \
		$$(LDFLAGS_WATCOM386_WIN_SDL) fil {$$^} \
		resource $$(OBJDIR_WATCOM386_WIN_SDL)/$(1).res

$$(OBJDIR_WATCOM386_WIN_SDL)/%.o: %.c | $$(MAUG_DEPS)
	$$(MD) "$$(dir $$@)"
	INCLUDE=$(WATCOM)/h $$(CC_WATCOM386) \
		$$(CFLAGS_WATCOM386) $$(CFLAGS_WATCOM386_WIN) \
		$$(CFLAGS_WATCOM386_WIN_SDL) $$(CFLAGS_OPT_WATCOM) \
		-fo=$$@ $$(<:%.c=%)

CLEAN_TARGETS += $(1)nts.exe

endef

# Legacy
TGTWINSDL=$(TGT_WATCOM386_WIN_SDL)

# ---

define TGT_WATCOM_WIN_WIN16

CFLAGS_WATCOM_WIN_WIN16 += \
	-ml -4 -bt=windows -i=$(WATCOM)/h/win -DRETROFLAT_API_WIN16 \
	-i=$(MAUG_ROOT)/api/retro2d/win -i=$(MAUG_ROOT)/api/input/win \
	-i=$(MAUG_ROOT)/api/file/unix -i=$(MAUG_ROOT)/api/log/unix \
	-i=$(MAUG_ROOT)/api/font/win

LDFLAGS_WATCOM_WIN_WIN16 += system windows option eliminate

ifeq ("$$(RETROFLAT_SOUND)","1")
	CFLAGS_WATCOM_WIN_WIN16 += -i=$(MAUG_ROOT)/api/sound/win
endif

# Strip out options to bake-in resources, as they make the exe too big!

CFLAGS_OPT_WATCOM_WIN16 := \
	$$(subst -DRETROFLAT_XPM -i=$$(RETROXPM_H_DIR),,$$(CFLAGS_OPT_WATCOM))

CFLAGS_OPT_WATCOM_WIN16 := \
	$$(subst -DMVFS_ENABLED -i=$$(MVFS_H_DIR),,$$(CFLAGS_OPT_WATCOM_WIN16))

MAUG_DEPS_WIN16 := $$(subst $$(RETROXPM_H_DIR)/xpmasset.h,,$$(MAUG_DEPS))

# Icon stuff.

$(eval $(call TGT_ICO_WIN,$(1),$$(OBJDIR_WATCOM_WIN_WIN16)))

$$(OBJDIR_WATCOM_WIN_WIN16)/$(1).res: $$(OBJDIR_WATCOM_WIN_WIN16)/$(1).rc
	wrc -r $$< -bt=windows -fo=$$@

$(1)w3.exe: \
$$(addprefix $$(OBJDIR_WATCOM_WIN_WIN16)/,$$(subst .c,.o,$$(C_FILES) $$(C_MAUG_STUBS))) \
$$(addprefix $$(OBJDIR_WATCOM_WIN_WIN16)/,$$(subst .c,.o,$$(C_MPLUG_OVLS))) | \
$$(OBJDIR_WATCOM_WIN_WIN16)/$(1).res
	$$(LD_WATCOM) name $$@ option dosseg option map \
		$$(LDFLAGS_WATCOM) $$(LDFLAGS_WATCOM_WIN) \
		$$(LDFLAGS_WATCOM_WIN_WIN16) fil {$$^}
# TODO: When we use a resource file, we get invalid segment for flat memory
# model errors?
# resource $$(OBJDIR_WATCOM_WIN_WIN16)/$(1).res

$$(OBJDIR_WATCOM_WIN_WIN16)/%.o: %.c | $$(MAUG_DEPS_WIN16)
	$$(MD) "$$(dir $$@)"
	INCLUDE=$(WATCOM)/h $(CC_WATCOM) \
		$$(CFLAGS_WATCOM) $$(CFLAGS_WATCOM_WIN) $$(CFLAGS_WATCOM_WIN_WIN16) \
		$$(CFLAGS_OPT_WATCOM_WIN16) \
		-fo="$$@" "$$(<:%.c=%)"

CLEAN_TARGETS += $(1)w3.exe

endef

# Legacy
TGTWIN16=$(TGT_WATCOM_WIN_WIN16)

# ---

define TGT_WATCOM386_WIN_WIN16

CFLAGS_WATCOM386_WIN_WIN16 += \
	-bt=windows -4r -i=$(WATCOM)/h/win -DRETROFLAT_API_WIN16 \
	-i=$(MAUG_ROOT)/api/retro2d/win -i=$(MAUG_ROOT)/api/input/win \
	-i=$(MAUG_ROOT)/api/file/unix -i=$(MAUG_ROOT)/api/log/unix \
	-i=$(MAUG_ROOT)/api/font/win

LDFLAGS_WATCOM386_WIN_WIN16 += system win386

ifeq ("$$(RETROFLAT_SOUND)","1")
	CFLAGS_WATCOM386_WIN_WIN16 += -i=$(MAUG_ROOT)/api/sound/win
endif

$(eval $(call TGT_ICO_WIN,$(1),$$(OBJDIR_WATCOM386_WIN_WIN16)))

# No resource compiler stanza because that's handled by the rex > exe xform
# below.

$(1)w.rex: \
$$(addprefix $$(OBJDIR_WATCOM386_WIN_WIN16)/,$$(subst .c,.o,$$(C_FILES) $$(C_MAUG_STUBS))) | \
$$(OBJDIR_WATCOM386_WIN_WIN16)/$(1).rc
	$$(LD_WATCOM386) name $$@ \
		$$(LDFLAGS_WATCOM386) \
		$$(LDFLAGS_WATCOM386_WIN) \
		$$(LDFLAGS_WATCOM386_WIN_WIN16) fil {$$^}

$(1)w.exe: $(1)w.rex
	wbind "$$<" -s "$$(WATCOM)/binw/win386.ext" -R \
	$$(OBJDIR_WATCOM386_WIN_WIN16)/$(1).rc "$$@"

$$(OBJDIR_WATCOM386_WIN_WIN16)/%.o: %.c | $$(MAUG_DEPS)
	$$(MD) "$$(dir $$@)"
	INCLUDE=$(WATCOM)/h $$(CC_WATCOM386) \
		$$(CFLAGS_WATCOM386) \
		$$(CFLAGS_WATCOM386_WIN) \
		$$(CFLAGS_WATCOM386_WIN_WIN16) \
		$$(CFLAGS_OPT_WATCOM) \
		-fo="$$@" "$$(<:%.c=%)"

CLEAN_TARGETS += $(1)w.exe $(1)w.rex

endef

# Legacy
TGTWIN386=$(TGT_WATCOM386_WIN_WIN16)

#---

define TGT_GCC_WIN_WIN32

CFLAGS_GCC_WIN_WIN32 += -I$(MAUG_ROOT)/api/retro2d/win \
	-I$(MAUG_ROOT)/api/input/win -I$(MAUG_ROOT)/api/file/win \
	-I$(MAUG_ROOT)/api/log/unix -i=$(MAUG_ROOT)/api/font/win

ifeq ("$$(RETROFLAT_VDP)","1")
	CFLAGS_GCC_WIN_WIN32 += -DRETROFLAT_VDP
endif

ifeq ("$$(RETROFLAT_SOUND)","1")
	CFLAGS_GCC_WIN_WIN32 += -I$(MAUG_ROOT)/api/sound/win
endif

$(eval $(call TGT_ICO_WIN,$(1),$$(OBJDIR_GCC_WIN_WIN32)))

$$(OBJDIR_GCC_WIN_WIN32)/$(1).res: $$(OBJDIR_GCC_WIN_WIN32)/$(1).rc
	$(WINDRES_GCC_MINGW) "$$<" -O coff -o "$$@"

$(1)ntg.exe: \
$$(addprefix $$(OBJDIR_GCC_WIN_WIN32)/,$$(subst .c,.o,$$(C_FILES))) \
$$(OBJDIR_GCC_WIN_WIN32)/$(1).res
	$$(LD_GCC_MINGW) -o $$@ $$^ $$(LDFLAGS_GCC_WIN) $$(LDFLAGS_GCC_WIN_WIN32)

$$(OBJDIR_GCC_WIN_WIN32)/%.o: %.c | $$(MAUG_DEPS)
	$$(MD) "$$(dir $$@)"
	$$(CC_GCC_MINGW) \
		$$(INCLUDES_GCC) $$(DEFINES_GCC) \
		$$(CFLAGS_GCC_WIN) $$(CFLAGS_GCC_WIN_WIN32) $$(CFLAGS_OPT_GCC) \
		-o $$@ -c $$<

CLEAN_TARGETS += $(1)ntg.exe

endef

# Legacy
TGTWINNTGCC=$(TGT_GCC_WIN_WIN32)

# ---

define TGT_GCC64_WIN_WIN32

CFLAGS_GCC64_WIN_WIN32 += -I$(MAUG_ROOT)/api/retro2d/win \
	-I$(MAUG_ROOT)/api/input/win -I$(MAUG_ROOT)/api/file/win \
	-I$(MAUG_ROOT)/api/log/unix -i=$(MAUG_ROOT)/api/font/win

ifeq ("$$(RETROFLAT_VDP)","1")
	CFLAGS_GCC64_WIN_WIN32 += -DRETROFLAT_VDP
endif

ifeq ("$$(RETROFLAT_OPENGL)","1")
	LDFLAGS_GCC64_WIN_WIN32 += -lopengl32
endif

ifeq ("$$(RETROFLAT_SOUND)","1")
	CFLAGS_GCC64_WIN_WIN32 += -I$(MAUG_ROOT)/api/sound/win
endif

$(eval $(call TGT_ICO_WIN,$(1),$$(OBJDIR_GCC64_WIN_WIN32)))

$$(OBJDIR_GCC64_WIN_WIN32)/$(1).res: $$(OBJDIR_GCC64_WIN_WIN32)/$(1).rc
	$(WINDRES_GCC64_MINGW) "$$<" -O coff -o "$$@"

# GCC64 for Windows gives undefined references to __fentry__ -g/-pg are used?

$(1)64.exe: \
$$(addprefix $$(OBJDIR_GCC64_WIN_WIN32)/,$$(subst .c,.o,$$(C_FILES))) \
$$(OBJDIR_GCC64_WIN_WIN32)/$(1).res
	$$(LD_GCC_MINGW64) -m64 -o $$@ $$^ \
		$$(subst -pg,,$$(subst -g,,$$(LDFLAGS_GCC64))) \
		$$(LDFLAGS_GCC64_WIN) $$(LDFLAGS_GCC64_WIN_WIN32)

$$(OBJDIR_GCC64_WIN_WIN32)/%.o: %.c | $$(MAUG_DEPS)
	$$(MD) "$$(dir $$@)"
	$$(CC_GCC_MINGW64) -m64 \
		$$(subst -pg,,$$(subst -g,,$$(CFLAGS_GCC64))) \
		$$(CFLAGS_GCC64_WIN) \
		$$(CFLAGS_GCC64_WIN_WIN32) \
		$$(CFLAGS_OPT_GCC) \
		-o $$@ -c $$<

CLEAN_TARGETS += $(1)64.exe

endef

# Legacy
TGTWIN64GCC=$(TGT_GCC64_WIN_WIN32)

