
# vim: ft=make noexpandtab

# Setup default CFLAGS/LDFLAGS for all Windows targets.
ifeq ("$(CFLAGS_WATCOM_WIN)","")
	CFLAGS_WATCOM_WIN := -DRETROFLAT_OS_WIN 
endif
ifeq ("$(LDFLAGS_WATCOM_WIN)","")
	LDFLAGS_WATCOM_WIN :=
endif
ifeq ("$(CFLAGS_GCC_WIN)","")
	CFLAGS_GCC_WIN := -DRETROFLAT_OS_WIN 
endif
ifeq ("$(LDFLAGS_GCC_WIN)","")
	LDFLAGS_GCC_WIN :=
endif

ifeq ("$(SCREENSAVER)","1")
	CFLAGS_WATCOM_WIN += -DRETROFLAT_SCREENSAVER
	CFLAGS_GCC_WIN += -DRETROFLAT_SCREENSAVER
endif

ifeq ("$(CC_GCC_MINGW)","")
	CC_GCC_MINGW = i686-w64-mingw32-gcc
endif
ifeq ("$(LD_GCC_MINGW)","")
	LD_GCC_MINGW = i686-w64-mingw32-gcc
endif

ifeq ("$(CC_GCC_MINGW64)","")
	CC_GCC_MINGW64 = x86_64-w64-mingw32-gcc
endif
ifeq ("$(LD_GCC_MINGW64)","")
	LD_GCC_MINGW64 = x86_64-w64-mingw32-gcc
endif

# ---

define TGTWINNT

CFLAGS_WATCOM_WIN_NT := -bt=nt -i$$(WATCOM)/h/nt -DRETROFLAT_API_WIN32
LDFLAGS_WATCOM_WIN_NT := system nt_win

ifeq ("$$(OPENGL)","1")
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

#---

define TGTWINNTGCC

CFLAGS_GCC_WIN_NT := -DRETROFLAT_API_WIN32
LDFLAGS_GCC_WIN_NT := -lgdi32

ifeq ("$$(OPENGL)","1")
	LDFLAGS_GCC_WIN_NT += -lopengl32
endif

$(1)ntg.exe: $$(addprefix obj/ntgcc/,$$(subst .c,.o,$$(C_FILES)))
	$$(LD_GCC_MINGW) -o $$@ $$^ $$(LDFLAGS_GCC_WIN) $$(LDFLAGS_GCC_WIN_NT)

obj/ntgcc/%.o: %.c | $(2)
	$$(MD) $$(dir $$@)
	$$(CC_GCC_MINGW) \
		$$(GLOBAL_DEFINES) $$(INCLUDES_GCC) $$(DEFINES_GCC) \
		$$(CFLAGS_GCC_WIN) $$(CFLAGS_GCC_WIN_NT) \
		-o $$@ -c $$<

CLEAN_TARGETS += $(1)ntg.exe $(2)

endef

#---

define TGTWIN64GCC

CFLAGS_GCC_WIN_64 := -DRETROFLAT_API_WIN32
LDFLAGS_GCC_WIN_64 := -lgdi32

ifeq ("$$(OPENGL)","1")
	LDFLAGS_GCC_WIN_64 += -lopengl32
endif

$(1)64.exe: $$(addprefix obj/nt64/,$$(subst .c,.o,$$(C_FILES)))
	$$(LD_GCC_MINGW64) -m64 -o $$@ $$^ $$(LDFLAGS_GCC_WIN) $$(LDFLAGS_GCC_WIN_64)

obj/nt64/%.o: %.c | $(2)
	$$(MD) $$(dir $$@)
	$$(CC_GCC_MINGW64) -m64 \
		$$(GLOBAL_DEFINES) $$(INCLUDES_GCC) $$(DEFINES_GCC) \
		$$(CFLAGS_GCC_WIN) $$(CFLAGS_GCC_WIN_64) \
		-o $$@ -c $$<

CLEAN_TARGETS += $(1)64.exe $(2)

endef

