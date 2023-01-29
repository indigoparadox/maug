
# vim: ft=make noexpandtab

# Setup default CFLAGS/LDFLAGS for all Windows targets.
ifeq ("$(CFLAGS_WATCOM_WIN)","")
	CFLAGS_WATCOM_WIN := -DRETROFLAT_OS_WIN 
endif
ifeq ("$(LDFLAGS_WATCOM_WIN)","")
	LDFLAGS_WATCOM_WIN :=
endif

ifeq ("$(SCREENSAVER)","1")
	CFLAGS_WATCOM_WIN += -DRETROFLAT_SCREENSAVER
endif

# ---

define TGTWINNT

CFLAGS_WATCOM_WIN_NT := -bt=nt -i$$(WATCOM)/h/nt -DRETROFLAT_API_WIN32
LDFLAGS_WATCOM_WIN_NT := system nt_win

ifeq ("$$(OPENGL)","1")
	CFLAGS_WATCOM_WIN_NT += -DRETROFLAT_OPENGL
	LDFLAGS_WATCOM_WIN_NT += libr opengl32
endif

$(1)nt.exe: $$(addprefix obj/nt/,$$(subst .c,.o,$$(C_FILES)))
	$$(LD_WATCOM) name $$@ \
		$$(LDFLAGS_WATCOM_WIN) $$(LDFLAGS_WATCOM_WIN_NT) fil {$$^}

obj/nt/%.o: %.c | $(2)
	$$(MD) $$(dir $$@)
	$$(CC_WATCOM) \
		$$(CFLAGS_WATCOM) $$(CFLAGS_WATCOM_WIN) $$(CFLAGS_WATCOM_WIN_NT) \
		-fo=$$@ $$(<:%.c=%)

CLEAN_TARGETS += $(1)nt.exe $(2)

endef

# ---

define TGTWIN386

CFLAGS_WATCOM_WIN_386 := -bt=windows -i$(WATCOM)/h/win -DRETROFLAT_API_WIN16
LDFLAGS_WATCOM_WIN_386 := system win386

$(1)w.rex: $(addprefix obj/win16/,$(subst .c,.o,$(C_FILES)))
	$$(LD_WATCOM) name $$@ \
		$$(LDFLAGS_WATCOM_WIN) $$(LDFLAGS_WATCOM_WIN_386) fil {$$^}

$(1)w.exe: $(1)w.rex
	wbind $$< -s $$(WATCOM)/binw/win386.ext -R $$@

obj/win16/%.o: %.c | $(2)
	$$(MD) $$(dir $$@)
	$$(CC_WATCOM) \
		$$(CFLAGS_WATCOM) $$(CFLAGS_WATCOM_WIN) $$(CFLAGS_WATCOM_WIN_386) \
		-fo=$$@ $$(<:%.c=%)

CLEAN_TARGETS += $(1)w.exe $(1)w.rex $(2)

endef

