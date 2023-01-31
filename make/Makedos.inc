
# vim: ft=make noexpandtab

# Setup default CFLAGS/LDFLAGS for all Windows targets.
ifeq ("$(CFLAGS_WATCOM_DOS)","")
	CFLAGS_WATCOM_DOS := -DRETROFLAT_OS_DOS 
endif
ifeq ("$(LDFLAGS_WATCOM_DOS)","")
	LDFLAGS_WATCOM_DOS :=
endif

# ---

define TGTDOSALE

CFLAGS_WATCOM_DOS_ALLEGRO := -bt=nt -s -3s -DRETROFLAT_API_ALLEGRO
LDFLAGS_WATCOM_DOS_ALLEGRO := system dos32a option stack=128k libr alleg

$(1)d.exe: $$(addprefix obj/dos/,$$(subst .c,.o,$$(C_FILES)))
	$$(LD_WATCOM) name $$@ \
		$$(LDFLAGS_WATCOM_DOS) $$(LDFLAGS_WATCOM_DOS_ALLEGRO) fil {$$^}

obj/dos/%.o: %.c | $(2)
	$$(MD) $$(dir $$@)
	$$(CC_WATCOM) \
		$$(CFLAGS_WATCOM) $$(CFLAGS_WATCOM_DOS) $$(CFLAGS_WATCOM_DOS_ALLEGRO) \
		-fo=$$@ $$(<:%.c=%)

CLEAN_TARGETS += $(1)d.exe $(2)

endef

