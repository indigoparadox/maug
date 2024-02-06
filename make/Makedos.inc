
# vim: ft=make noexpandtab

# Setup default CFLAGS/LDFLAGS for all DOS targets.
ifeq ("$(CFLAGS_WATCOM386_DOS)","")
	CFLAGS_WATCOM386_DOS := -DRETROFLAT_OS_DOS -DRETROSND_API_PC_BIOS
endif
ifeq ("$(LDFLAGS_WATCOM386_DOS)","")
	LDFLAGS_WATCOM386_DOS :=
endif
ifeq ("$(CFLAGS_WATCOM_DOS)","")
	CFLAGS_WATCOM_DOS := -DRETROFLAT_OS_DOS_REAL -DRETROSND_API_PC_BIOS -DRETROSND_ARGS
	CFLAGS_WATCOM_DOS += -DLOG_TO_FILE -DLOG_FILE_NAME=\"out.log\"
endif
ifeq ("$(LDFLAGS_WATCOM_DOS)","")
	LDFLAGS_WATCOM_DOS :=
endif

C_MAUG_STUBS := $(wildcard maug/dosstubs/*.c)

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

define TGTDOSBIOS

ifeq ("$$(RETROFLAT_DOS_MEM_LARGE)", "1")
   CFLAGS_WATCOM_DOS_BIOS_M := -ml -DRETROFLAT_DOS_MEM_L
else
	CFLAGS_WATCOM_DOS_BIOS_M := -mm
endif

ifeq ("$$(CFLAGS_WATCOM_DOS_BIOS)","")
	CFLAGS_WATCOM_DOS_BIOS := \
		-bt=dos -DRETROFLAT_API_PC_BIOS -DRETROFLAT_DEPTH_BIOS
endif
ifeq ("$$(LDFLAGS_WATCOM_DOS_ALLEGRO)","")
	LDFLAGS_WATCOM_DOS_BIOS := system dos option stack=8k
endif

$(1)b.exe: $$(addprefix obj/dos_bios/,$$(subst .c,.o,$$(C_FILES) $$(C_MAUG_STUBS)))
	$$(LD_WATCOM) name $$@ option dosseg \
		$$(LDFLAGS_WATCOM) $$(LDFLAGS_WATCOM_DOS) \
		$$(LDFLAGS_WATCOM_DOS_BIOS) fil {$$^}

obj/dos_bios/%.o: %.c | $(MAUG_DEPS)
	$$(MD) $$(dir $$@)
	$$(CC_WATCOM) \
		$$(CFLAGS_WATCOM) $$(CFLAGS_WATCOM_DOS_BIOS_M) $$(CFLAGS_WATCOM_DOS) \
		$$(CFLAGS_WATCOM_DOS_BIOS) \
		-fo=$$@ $$(<:%.c=%)

CLEAN_TARGETS += $(1)b.exe

endef

