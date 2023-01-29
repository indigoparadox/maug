
# vim: ft=make noexpandtab

define TGTWINNT

ifeq ("$$(OPENGL)","1")
	CFLAGS_WATCOM_NT += -DRETROFLAT_OPENGL
	LDFLAGS_WATCOM_NT += libr opengl32
endif

$(1)nt.exe: $$(addprefix obj/nt/,$$(subst .c,.o,$$(C_FILES)))
	$$(LD_WATCOM) name $$@ system nt_win $$(LDFLAGS_WATCOM_NT) fil {$$^}

obj/nt/%.o: %.c
	$$(MD) $$(dir $$@)
	$$(CC_WATCOM) -bt=nt -i$$(WATCOM)/h/nt -DRETROFLAT_API_WIN32 -DRETROFLAT_OS_WIN $$(CFLAGS_WATCOM) $$(CFLAGS_WATCOM_NT) -fo=$$@ $$(<:%.c=%)

endef

