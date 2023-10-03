
# vim: ft=make noexpandtab

# Setup default CFLAGS/LDFLAGS for all DOS targets.
ifeq ("$(CFLAGS_WATCOM386_DOS)","")
	CFLAGS_WATCOM386_DOS := -DRETROFLAT_OS_DOS 
	CFLAGS_WATCOM_DOS := -DRETROFLAT_OS_DOS 
endif
ifeq ("$(LDFLAGS_WATCOM386_DOS)","")
	LDFLAGS_WATCOM_DOS :=
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

# ---

define TGTDOSCGA

ifeq ("$$(CFLAGS_WATCOM_DOS_CGA)","")
	CFLAGS_WATCOM_DOS_CGA := \
		-bt=dos -DRETROFLAT_API_CGA
endif
ifeq ("$$(LDFLAGS_WATCOM_DOS_ALLEGRO)","")
	LDFLAGS_WATCOM_DOS_CGA := system dos
endif

$(1)c.exe: $$(addprefix obj/dos_cga/,$$(subst .c,.o,$$(C_FILES)))
	$$(LD_WATCOM) name $$@ \
		$$(LDFLAGS_WATCOM) $$(LDFLAGS_WATCOM_DOS) \
		$$(LDFLAGS_WATCOM_DOS_CGA) fil {$$^}

obj/dos_cga/%.o: %.c | $(MAUG_DEPS)
	$$(MD) $$(dir $$@)
	$$(CC_WATCOM) \
		$$(CFLAGS_WATCOM) $$(CFLAGS_WATCOM_DOS) \
		$$(CFLAGS_WATCOM_DOS_CGA) \
		-fo=$$@ $$(<:%.c=%)

CLEAN_TARGETS += $(1)c.exe

endef

