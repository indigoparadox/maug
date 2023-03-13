
# vim: ft=make noexpandtab

# Setup default CFLAGS/LDFLAGS for all DOS targets.
ifeq ("$(CFLAGS_WATCOM386_DOS)","")
	CFLAGS_WATCOM386_DOS := -DRETROFLAT_OS_DOS 
endif
ifeq ("$(LDFLAGS_WATCOM386_DOS)","")
	LDFLAGS_WATCOM386_DOS :=
endif

# ---

define TGTDOSALE

ifeq ("$$(CFLAGS_WATCOM386_DOS_ALLEGRO)","")
	CFLAGS_WATCOM386_DOS_ALLEGRO := \
		-bt=nt -s -3s -DRETROFLAT_API_ALLEGRO
endif
ifeq ("$$(LDFLAGS_WATCOM386_DOS_ALLEGRO)","")
	LDFLAGS_WATCOM386_DOS_ALLEGRO := \
		system dos32a option stack=128k libr alleg
endif

$(1)d.exe: $$(addprefix obj/dos/,$$(subst .c,.o,$$(C_FILES)))
	$$(LD_WATCOM386) name $$@ \
		$$(LDFLAGS_WATCOM386) $$(LDFLAGS_WATCOM386_DOS) \
		$$(LDFLAGS_WATCOM386_DOS_ALLEGRO) fil {$$^}

obj/dos/%.o: %.c | $(MAUG_DEPS)
	$$(MD) $$(dir $$@)
	$$(CC_WATCOM386) \
		$$(CFLAGS_WATCOM386) $$(CFLAGS_WATCOM386_DOS) \
		$$(CFLAGS_WATCOM386_DOS_ALLEGRO) \
		-fo=$$@ $$(<:%.c=%)

CLEAN_TARGETS += $(1)d.exe

endef

