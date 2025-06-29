
# vim: ft=make noexpandtab

ifeq ("$(CFLAGS_WATCOM_OS2)","")
	CFLAGS_WATCOM_OS2 := -DRETROFLAT_OS_OS2
endif

CFLAGS_WATCOM386_OS2_GLUT := \
	-mf -5r -fp5 -bt=os2 -i$(WATCOM)/h/os2 -DRETROFLAT_API_GLUT \
	-i=$(MAUG_ROOT)/api/retro2d/glut -i=$(MAUG_ROOT)/api/input/glut \
	-i=$(MAUG_ROOT)/api/file/unix -i=$(MAUG_ROOT)/api/log/unix
LDFLAGS_WATCOM386_OS2_GLUT := system os2v2_pm libr {opengl glut}

CFLAGS_WATCOM386_OS2_SDL := \
	-mf -5s -fp5 -bt=os2 -i=$(WATCOM)/h/os2 -i=$(WATCOM)/h/os2/SDL \
	-DRETROFLAT_API_SDL1 \
	-i=$(MAUG_ROOT)/api/retro2d/sdl -i=$(MAUG_ROOT)/api/input/sdl \
	-i=$(MAUG_ROOT)/api/file/unix -i=$(MAUG_ROOT)/api/log/unix
LDFLAGS_WATCOM386_OS2_SDL := system os2v2_pm libr {sdl12}

OBJDIR_WATCOM386_OS2_GLUT=obj/watcom-os232-glut
OBJDIR_WATCOM386_OS2_SDL=obj/watcom-os232-sdl

# ---

define TGT_WATCOM386_OS2_GLUT

ifeq ("$$(RETROFLAT_SOUND)","1")
	CFLAGS_WATCOM386_OS2_GLUT += -i=$(MAUG_ROOT)/api/sound/null
endif

$(1)o32gl.exe: \
$$(addprefix $$(OBJDIR_WATCOM386_OS2_GLUT)/,$$(subst .c,.o,$$(C_FILES))) | \
$$(RES_WATCOM386_OS2_GLUT_DEP)
	$$(LD_WATCOM386) name $$@ \
		$$(LDFLAGS_WATCOM_OS2) $$(LDFLAGS_WATCOM386_OS2_GLUT) fil {$$^} \
		$$(RES_WATCOM386_OS2_GLUT)

$$(OBJDIR_WATCOM386_OS2_GLUT)/%.o: %.c | $$(MAUG_DEPS)
	$$(MD) "$$(dir $$@)"
	INCLUDE=$(WATCOM)/h $$(CC_WATCOM386) \
		$$(CFLAGS_WATCOM) $$(CFLAGS_WATCOM_OS2) $$(CFLAGS_WATCOM386_OS2_GLUT) \
		$$(CFLAGS_OPT_WATCOM) \
		-fo=$$@ $$(<:%.c=%)

CLEAN_TARGETS += $(1)o32gl.exe

endef

# Legacy
TGTOS2GL=$(TGT_WATCOM386_OS2_GLUT)

# ---

define TGT_WATCOM386_OS2_SDL

ifeq ("$$(RETROFLAT_SOUND)","1")
	CFLAGS_WATCOM386_OS2_SDL += -i=$(MAUG_ROOT)/api/sound/sdl
endif

$(1)o32s.exe: \
$$(addprefix $$(OBJDIR_WATCOM386_OS2_SDL)/,$$(subst .c,.o,$$(C_FILES))) | \
$$(RES_WATCOM386_OS2_SDL_DEP)
	$$(LD_WATCOM386) name $$@ \
		$$(LDFLAGS_WATCOM_OS2) $$(LDFLAGS_WATCOM386_OS2_SDL) fil {$$^} \
		$$(RES_WATCOM386_OS2_SDL)

$$(OBJDIR_WATCOM386_OS2_SDL)/%.o: %.c | $$(MAUG_DEPS)
	$$(MD) "$$(dir $$@)"
	INCLUDE=$(WATCOM)/h $$(CC_WATCOM386) \
		$$(CFLAGS_WATCOM) $$(CFLAGS_WATCOM_OS2) $$(CFLAGS_WATCOM386_OS2_SDL) \
		$$(CFLAGS_OPT_WATCOM) \
		-fo=$$@ $$(<:%.c=%)

CLEAN_TARGETS += $(1)o32s.exe

endef

# Legacy
TGTOS2SDL=$(TGT_WATCOM386_OS2_SDL)

