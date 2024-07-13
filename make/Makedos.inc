
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
CFLAGS_DJGPP_DOS := -DRETROFLAT_OS_DOS -DRETROSND_API_PC_BIOS

ifeq ($(CC_DJGPP),)
	CC_DJGPP := i586-pc-msdosdjgpp-gcc
endif
ifeq ($(LD_DJGPP),)
	LD_DJGPP := i586-pc-msdosdjgpp-gcc
endif
CFLAGS_DJGPP += $(GLOBAL_DEFINES) $(INCLUDES_GCC)

C_MAUG_STUBS := $(wildcard maug/dosstubs/*.c)

# ---

define TGTDOSALE

CFLAGS_WATCOM386_DOS_ALLEGRO += \
	-bt=nt -s -3s -DRETROFLAT_API_ALLEGRO -I$(MAUG_ROOT)/api/allegro

LDFLAGS_WATCOM386_DOS_ALLEGRO += \
	system dos32a option stack=128k libr alleg

$(1)d.exe: $$(addprefix obj/doswa/,$$(subst .c,.o,$$(C_FILES)))
	$$(LD_WATCOM386) name $$@ \
		$$(LDFLAGS_WATCOM386) $$(LDFLAGS_WATCOM386_DOS) \
		$$(LDFLAGS_WATCOM386_DOS_ALLEGRO) fil {$$^}

obj/doswa/%.o: %.c | $$(MAUG_DEPS)
	$$(MD) $$(dir $$@)
	$$(CC_WATCOM386) \
		$$(CFLAGS_WATCOM386) $$(CFLAGS_WATCOM386_DOS) \
		$$(CFLAGS_WATCOM386_DOS_ALLEGRO) $$(CFLAGS_OPT_WATCOM) \
		-fo=$$@ $$(<:%.c=%)

CLEAN_TARGETS += $(1)d.exe

endef

# ---

define TGTDOSGAL

CFLAGS_DJGPP_DOS_ALLEGRO += -DRETROFLAT_API_ALLEGRO -I$(MAUG_ROOT)/api/allegro

LDFLAGS_DJGPP_DOS_ALLEGRO += -lalleg

$(1)dg.exe: $$(addprefix obj/dosdg/,$$(subst .c,.o,$$(C_FILES)))
	$$(LD_DJGPP) -o $$@ $$^ \
		$$(LDFLAGS_DJGPP) $$(LDFLAGS_DJGPP_DOS) \
		$$(LDFLAGS_DJGPP_DOS_ALLEGRO)

obj/dosdg/%.o: %.c | $$(MAUG_DEPS)
	$$(MD) $$(dir $$@)
	$$(CC_DJGPP) \
		$$(CFLAGS_DJGPP) $$(CFLAGS_DJGPP_DOS) \
		$$(CFLAGS_DJGPP_DOS_ALLEGRO) $$(CFLAGS_OPT_GCC) \
		-o $$@ -c $$<

CLEAN_TARGETS += $(1)dg.exe

endef

# ---

define TGTDOSBIOS

CFLAGS_WATCOM_DOS_BIOS += -I$(MAUG_ROOT)/api/dosbios

ifeq ("$$(RETROFLAT_DOS_MEM_LARGE)", "1")
	CFLAGS_WATCOM_DOS_BIOS += -ml -DMAUG_DOS_MEM_L
else
	CFLAGS_WATCOM_DOS_BIOS += -mm
endif

CFLAGS_WATCOM_DOS_BIOS += \
	-bt=dos -DRETROFLAT_API_PC_BIOS -DRETROFLAT_DEPTH_BIOS

LDFLAGS_WATCOM_DOS_BIOS += system dos option stack=8k

$(1)b.exe: $$(addprefix obj/dos_bios/,$$(subst .c,.o,$$(C_FILES) $$(C_MAUG_STUBS))) $$(addprefix obj/dos_bios/,$$(subst .c,.o,$$(C_MPLUG_OVLS)))
	$$(LD_WATCOM) name $$@ option dosseg option map \
		$$(LDFLAGS_WATCOM) $$(LDFLAGS_WATCOM_DOS) \
		$$(LDFLAGS_WATCOM_DOS_BIOS) fil {$$^}

obj/dos_bios/%.o: %.c | $$(MAUG_DEPS)
	$$(MD) $$(dir $$@)
	$$(CC_WATCOM) \
		$$(CFLAGS_WATCOM) $$(CFLAGS_WATCOM_DOS) $$(CFLAGS_WATCOM_DOS_BIOS) \
		$$(CFLAGS_OPT_WATCOM) \
		-fo=$$@ $$(<:%.c=%)

CLEAN_TARGETS += $(1)b.exe

endef

