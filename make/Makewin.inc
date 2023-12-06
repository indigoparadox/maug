
# vim: ft=make noexpandtab

# Setup default CFLAGS/LDFLAGS for all Windows targets.
ifeq ("$(CFLAGS_WATCOM_WIN)","")
	CFLAGS_WATCOM_WIN := -DRETROFLAT_OS_WIN -DRETROSND_API_WINMM
endif
ifeq ("$(LDFLAGS_WATCOM_WIN)","")
	LDFLAGS_WATCOM_WIN := libr mmsystem
endif

ifeq ("$(CFLAGS_WATCOM386_WIN)","")
	CFLAGS_WATCOM386_WIN := -DRETROFLAT_OS_WIN  -DRETROSND_API_WINMM
endif
ifeq ("$(LDFLAGS_WATCOM386_WIN)","")
	LDFLAGS_WATCOM386_WIN := libr winmm
endif

ifeq ("$(CFLAGS_GCC_WIN)","")
	CFLAGS_GCC_WIN := -march=i486 -DRETROFLAT_OS_WIN -DRETROSND_API_WINMM
endif
ifeq ("$(LDFLAGS_GCC_WIN)","")
	LDFLAGS_GCC_WIN := -mwindows -lgdi32 -lwinmm
endif

ifeq ("$(CFLAGS_GCC64_WIN)","")
	CFLAGS_GCC64_WIN := -DRETROFLAT_OS_WIN  -DRETROSND_API_WINMM
endif
ifeq ("$(LDFLAGS_GCC64_WIN)","")
	LDFLAGS_GCC64_WIN := -mwindows -lgdi32
endif

ifeq ("$(RELEASE)","RELEASE")
	CFLAGS_GCC_WIN += -O2
	CFLAGS_GCC64_WIN += -O2
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

# Common Variables

# TODO: Right now, these must be specified before inclusion and apply to all
# Windows targets... maybe make them per-target instead?

ifeq ("$(SCREENSAVER)","1")
	CFLAGS_WATCOM_WIN += -DRETROFLAT_SCREENSAVER
	CFLAGS_WATCOM386_WIN += -DRETROFLAT_SCREENSAVER
	CFLAGS_GCC_WIN += -DRETROFLAT_SCREENSAVER
	CFLAGS_GCC64_WIN += -DRETROFLAT_SCREENSAVER
endif

ifeq ("$(OPENGL)","1")
	LDFLAGS_WATCOM386_WIN += libr opengl32
	LDFLAGS_GCC_WIN += -lopengl32
	LDFLAGS_GCC64_WIN += -lopengl32
endif

ifeq ("$(WING)","1")
	CFLAGS_WATCOM_WIN += -DRETROFLAT_WING
	CFLAGS_WATCOM386_WIN += -DRETROFLAT_WING
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

# ---

define TGT_WATCOM386_WIN_WIN32

ifeq ("$$(CFLAGS_WATCOM386_WIN_WIN32)","")
	CFLAGS_WATCOM386_WIN_WIN32 := \
		-mf -5r -fp5 -bt=nt -i$(WATCOM)/h/nt -DRETROFLAT_API_WIN32
endif
ifeq ("$$(LDFLAGS_WATCOM386_WIN_WIN32)","")
	LDFLAGS_WATCOM386_WIN_WIN32 := system nt_win
endif

ifeq ("$$(VDP)","1")
	CFLAGS_WATCOM386_WIN_WIN32 += -DRETROFLAT_VDP
endif

$(1)nt.exe: \
$$(addprefix $$(OBJDIR_WATCOM386_WIN_WIN32)/,$$(subst .c,.o,$$(C_FILES))) | \
$$(RES_WATCOM386_WIN_WIN32_DEP)
	$$(LD_WATCOM386) name $$@ \
		$$(LDFLAGS_WATCOM386) $$(LDFLAGS_WATCOM386_WIN) \
		$$(LDFLAGS_WATCOM386_WIN_WIN32) fil {$$^} \
		$$(RES_WATCOM386_WIN_WIN32)

$$(OBJDIR_WATCOM386_WIN_WIN32)/%.o: %.c | $(MAUG_DEPS)
	$$(MD) "$$(dir $$@)"
	$$(CC_WATCOM386) \
		$$(CFLAGS_WATCOM386) $$(CFLAGS_WATCOM386_WIN) \
		$$(CFLAGS_WATCOM386_WIN_WIN32) \
		-fo=$$@ $$(<:%.c=%)

CLEAN_TARGETS += $(1)nt.exe

endef

# Legacy
TGTWINNT=$(TGT_WATCOM386_WIN_WIN32)

# ---

define TGT_WATCOM386_WIN_GLUT

ifeq ("$$(CFLAGS_WATCOM386_WIN_GLUT)","")
	CFLAGS_WATCOM386_WIN_GLUT := \
		-mf -5r -fp5 -bt=nt -i$(WATCOM)/h/nt -DRETROFLAT_API_GLUT
endif
ifeq ("$$(LDFLAGS_WATCOM386_WIN_GLUT)","")
	LDFLAGS_WATCOM386_WIN_GLUT := system nt_win
endif

$(1)ntgl.exe: \
$$(addprefix $$(OBJDIR_WATCOM386_WIN_GLUT)/,$$(subst .c,.o,$$(C_FILES))) | \
$$(RES_WATCOM386_WIN_GLUT_DEP)
	$$(LD_WATCOM386) name $$@ \
		$$(LDFLAGS_WATCOM386) $$(LDFLAGS_WATCOM386_WIN) \
		$$(LDFLAGS_WATCOM386_WIN_GLUT) fil {$$^} \
		$$(RES_WATCOM386_WIN_GLUT)

$$(OBJDIR_WATCOM386_WIN_GLUT)/%.o: %.c | $(MAUG_DEPS)
	$$(MD) "$$(dir $$@)"
	$$(CC_WATCOM386) \
		$$(CFLAGS_WATCOM386) $$(CFLAGS_WATCOM386_WIN) \
		$$(CFLAGS_WATCOM386_WIN_GLUT) \
		-fo=$$@ $$(<:%.c=%)

CLEAN_TARGETS += $(1)ntgl.exe

endef

# Legacy
TGTWINGL=$(TGT_WATCOM386_WIN_GLUT)

# ---

define TGT_WATCOM386_WIN_SDL

ifeq ("$$(CFLAGS_WATCOM386_WIN_SDL)","")
	CFLAGS_WATCOM386_WIN_SDL := \
		-mf -5s -ei -fp5 -dSDL_main=main -bt=nt \
		-i$(WATCOM)/h/nt -i$(WATCOM)/h/nt/SDL -DRETROFLAT_API_SDL1
endif
ifeq ("$$(LDFLAGS_WATCOM386_WIN_SDL)","")
	LDFLAGS_WATCOM386_WIN_SDL := system nt_win libr {SDL SDLmain}
endif

$(1)nts.exe: \
$$(addprefix $$(OBJDIR_WATCOM386_WIN_SDL)/,$$(subst .c,.o,$$(C_FILES))) | \
$$(RES_WATCOM386_WIN_SDL_DEP)
	$$(LD_WATCOM386) name $$@ \
		$$(LDFLAGS_WATCOM386) $$(LDFLAGS_WATCOM386_WIN) \
		$$(LDFLAGS_WATCOM386_WIN_SDL) fil {$$^} \
		$$(RES_WATCOM386_WIN_SDL)

$$(OBJDIR_WATCOM386_WIN_SDL)/%.o: %.c | $(MAUG_DEPS)
	$$(MD) "$$(dir $$@)"
	$$(CC_WATCOM386) \
		$$(CFLAGS_WATCOM386) $$(CFLAGS_WATCOM386_WIN) \
		$$(CFLAGS_WATCOM386_WIN_SDL) \
		-fo=$$@ $$(<:%.c=%)

CLEAN_TARGETS += $(1)nts.exe

endef

# Legacy
TGTWINSDL=$(TGT_WATCOM386_WIN_SDL)

# ---

define TGT_WATCOM_WIN_WIN16

ifeq ("$$(CFLAGS_WATCOM_WIN_WIN16)","")
	CFLAGS_WATCOM_WIN_WIN16 := \
		-ml -4 -bt=windows -i$(WATCOM)/h/win -DRETROFLAT_API_WIN16
endif
ifeq ("$$(LDFLAGS_WATCOM_WIN_WIN16)","")
	LDFLAGS_WATCOM_WIN_WIN16 := system windows
endif

$(1)w3.exe: \
$(addprefix $$(OBJDIR_WATCOM_WIN_WIN16)/,$(subst .c,.o,$(C_FILES))) | \
$$(RES_WATCOM_WIN_WIN16_DEP)
	$(LD_WATCOM) name $$@ \
		$$(LDFLAGS_WATCOM) $$(LDFLAGS_WATCOM_WIN) \
		$$(LDFLAGS_WATCOM_WIN_WIN16) fil {$$^} \
		$$(RES_WATCOM_WIN_WIN16)

$$(OBJDIR_WATCOM_WIN_WIN16)/%.o: %.c | $(MAUG_DEPS)
	$$(MD) "$$(dir $$@)"
	$(CC_WATCOM) \
		$$(CFLAGS_WATCOM) $$(CFLAGS_WATCOM_WIN) $$(CFLAGS_WATCOM_WIN_WIN16) \
		-fo="$$@" "$$(<:%.c=%)"

CLEAN_TARGETS += $(1)w3.exe

endef

# Legacy
TGTWIN16=$(TGT_WATCOM_WIN_WIN16)

# ---

define TGT_WATCOM386_WIN_WIN16

ifeq ("$$(CFLAGS_WATCOM386_WIN_WIN16)","")
	CFLAGS_WATCOM386_WIN_WIN16 := \
		-bt=windows -4r -i$(WATCOM)/h/win -DRETROFLAT_API_WIN16
endif
ifeq ("$$(LDFLAGS_WATCOM386_WIN_WIN16)","")
	LDFLAGS_WATCOM386_WIN_WIN16 := system win386
endif

$(1)w.rex: \
$(addprefix $$(OBJDIR_WATCOM386_WIN_WIN16)/,$(subst .c,.o,$(C_FILES))) | \
$$(RES_WATCOM386_WIN_WIN16_DEP)
	$$(LD_WATCOM386) name $$@ \
		$$(LDFLAGS_WATCOM386) \
		$$(LDFLAGS_WATCOM386_WIN) \
		$$(LDFLAGS_WATCOM386_WIN_WIN16) fil {$$^}

$(1)w.exe: $(1)w.rex
	wbind "$$<" -s "$$(WATCOM)/binw/win386.ext" -R \
	$$(RES_WATCOM386_WIN_WIN16_DEP) "$$@"

$$(OBJDIR_WATCOM386_WIN_WIN16)/%.o: %.c | $(MAUG_DEPS)
	$$(MD) "$$(dir $$@)"
	$$(CC_WATCOM386) \
		$$(CFLAGS_WATCOM386) \
		$$(CFLAGS_WATCOM386_WIN) \
		$$(CFLAGS_WATCOM386_WIN_WIN16) \
		-fo="$$@" "$$(<:%.c=%)"

CLEAN_TARGETS += $(1)w.exe $(1)w.rex

endef

# Legacy
TGTWIN386=$(TGT_WATCOM386_WIN_WIN16)

#---

define TGT_GCC_WIN_WIN32

ifeq ("$$(VDP)","1")
	CFLAGS_GCC_WIN_WIN32 += -DRETROFLAT_VDP
endif

$(1)ntg.exe: \
$$(addprefix $$(OBJDIR_GCC_WIN_WIN32)/,$$(subst .c,.o,$$(C_FILES))) \
$$(RES_GCC_WIN_WIN32_DEP)
	$$(LD_GCC_MINGW) -o $$@ $$^ $$(LDFLAGS_GCC_WIN) $$(LDFLAGS_GCC_WIN_WIN32)

$$(OBJDIR_GCC_WIN_WIN32)/%.o: %.c | $(MAUG_DEPS)
	$$(MD) "$$(dir $$@)"
	$$(CC_GCC_MINGW) \
		$$(GLOBAL_DEFINES) $$(INCLUDES_GCC) $$(DEFINES_GCC) \
		$$(CFLAGS_GCC_WIN) $$(CFLAGS_GCC_WIN_WIN32) \
		-o $$@ -c $$<

CLEAN_TARGETS += $(1)ntg.exe

endef

# Legacy
TGTWINNTGCC=$(TGT_GCC_WIN_WIN32)

# ---

define TGT_GCC64_WIN_WIN32

ifeq ("$$(VDP)","1")
	CFLAGS_GCC64_WIN_WIN32 += -DRETROFLAT_VDP
endif

ifeq ("$$(OPENGL)","1")
	LDFLAGS_GCC64_WIN_WIN32 += -lopengl32
endif

$(1)64.exe: \
$$(addprefix $$(OBJDIR_GCC64_WIN_WIN32)/,$$(subst .c,.o,$$(C_FILES))) \
$$(RES_GCC64_WIN_WIN32_DEP)
	$$(LD_GCC_MINGW64) -m64 -o $$@ $$^ \
		$$(LDFLAGS_GCC64_WIN) $$(LDFLAGS_GCC64_WIN_WIN32)

$$(OBJDIR_GCC64_WIN_WIN32)/%.o: %.c | $(MAUG_DEPS)
	$$(MD) "$$(dir $$@)"
	$$(CC_GCC_MINGW64) -m64 \
		$$(CFLAGS_GCC64) $$(CFLAGS_GCC64_WIN) $$(CFLAGS_GCC64_WIN_WIN32) \
		-o $$@ -c $$<

CLEAN_TARGETS += $(1)64.exe

endef

# Legacy
TGTWIN64GCC=$(TGT_GCC64_WIN_WIN32)

