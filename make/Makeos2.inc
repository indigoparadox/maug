
# vim: ft=make noexpandtab

ifeq ("$(CFLAGS_WATCOM_OS2)","")
	CFLAGS_WATCOM_OS2 := -DRETROFLAT_OS_OS2
endif

CFLAGS_WATCOM386_OS2_GLUT := \
	-mf -5r -fp5 -bt=os2v2_pm -i$(WATCOM)/h/os2 -DRETROFLAT_API_GLUT
LDFLAGS_WATCOM386_OS2_GLUT := system os2v2_pm

OBJDIR_WATCOM386_OS2_GLUT=obj/watcom-os232-glut

define TGT_WATCOM386_OS2_GLUT

ifeq ("$$(OPENGL)","1")
	LDFLAGS_WATCOM386_OS2_GLUT += libr {opengl glut}
endif

$(1)o32gl.exe: \
$$(addprefix $$(OBJDIR_WATCOM386_OS2_GLUT)/,$$(subst .c,.o,$$(C_FILES))) | \
$$(RES_WATCOM386_OS2_GLUT_DEP)
	$$(LD_WATCOM) name $$@ \
		$$(LDFLAGS_WATCOM_OS2) $$(LDFLAGS_WATCOM386_OS2_GLUT) fil {$$^} \
		$$(RES_WATCOM386_OS2_GLUT)

$$(OBJDIR_WATCOM386_OS2_GLUT)/%.o: %.c | $(MAUG_DEPS)
	$$(MD) "$$(dir $$@)"
	$$(CC_WATCOM) \
		$$(CFLAGS_WATCOM) $$(CFLAGS_WATCOM_OS2) $$(CFLAGS_WATCOM386_OS2_GLUT) \
		-fo=$$@ $$(<:%.c=%)

CLEAN_TARGETS += $(1)o32gl.exe

endef

# Legacy
TGTOS2GL=$(TGT_WATCOM386_OS2_GLUT)

