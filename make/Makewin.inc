
# vim: ft=make noexpandtab

# Setup default CFLAGS/LDFLAGS for all Windows targets.
ifeq ("$(CFLAGS_WATCOM_WIN)","")
	CFLAGS_WATCOM_WIN := -DRETROFLAT_OS_WIN 
endif
ifeq ("$(LDFLAGS_WATCOM_WIN)","")
	LDFLAGS_WATCOM_WIN :=
endif
ifeq ("$(CFLAGS_GCC_WIN)","")
	CFLAGS_GCC_WIN := -march=i486 -DRETROFLAT_OS_WIN 
endif
ifeq ("$(LDFLAGS_GCC_WIN)","")
	LDFLAGS_GCC_WIN := -mwindows -lgdi32
endif

ifeq ("$(RELEASE)","RELEASE")
	CFLAGS_GCC_WIN += -O2
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
ifeq ("$(WINDRES_GCC_MINGW)","")
	WINDRES_GCC_MINGW = i686-w64-mingw32-windres
endif

ifeq ("$(CC_GCC_MINGW64)","")
	CC_GCC_MINGW64 = x86_64-w64-mingw32-gcc
endif
ifeq ("$(LD_GCC_MINGW64)","")
	LD_GCC_MINGW64 = x86_64-w64-mingw32-gcc
endif
ifeq ("$(WINDRES_GCC64_MINGW)","")
	WINDRES_GCC64_MINGW = x86_64-w64-mingw32-windres
endif

# Common Variables

CFLAGS_WATCOM386_WIN_WIN16 := \
	-bt=windows -i$(WATCOM)/h/win -DRETROFLAT_API_WIN16
LDFLAGS_WATCOM386_WIN_WIN16 := system win386

CFLAGS_WATCOM_WIN_WIN32 := -bt=nt -i$(WATCOM)/h/nt -DRETROFLAT_API_WIN32
LDFLAGS_WATCOM_WIN_WIN32 := system nt_win
CFLAGS_GCC_WIN_WIN32 := -DRETROFLAT_API_WIN32
CFLAGS_GCC64_WIN_WIN32 := -DRETROFLAT_API_WIN32

OBJDIR_WATCOM_WIN_WIN32=obj/watcom-win-win32
OBJDIR_WATCOM386_WIN_WIN16=obj/watcom386-win-win16
OBJDIR_GCC_WIN_WIN32=obj/gcc-win-win32
OBJDIR_GCC64_WIN_WIN32=obj/gcc64-win-win32

# ---

ifeq ("$(TGTWINICO)","")
define TGTWINICO

# Create icon and resource file.

# Watcom-Win-Win32

RES_WATCOM_WIN_WIN32_DEP=$$(OBJDIR_WATCOM_WIN_WIN32)/$(1).res
RES_WATCOM_WIN_WIN32=resource {$$(RES_WATCOM_WIN_WIN32_DEP)}

$$(OBJDIR_WATCOM_WIN_WIN32)/$(1).ico: $(1).bmp
	convert "$$<" "$$@"

$$(OBJDIR_WATCOM_WIN_WIN32)/$(1).rc: $$(OBJDIR_WATCOM_WIN_WIN32)/$(1).ico
	echo "1000   ICON   \"$$(OBJDIR_WATCOM_WIN_WIN32)/$(1).ico\"" > $$@

$$(OBJDIR_WATCOM_WIN_WIN32)/$(1).res: $$(OBJDIR_WATCOM_WIN_WIN32)/$(1).rc
	wrc -r $$< -bt=nt -fo=$$@

# Watcom386-Win-Win32

$$(OBJDIR_WATCOM386_WIN_WIN16)/$(1).ico: $(1).bmp
	convert "$$<" "$$@"

$$(OBJDIR_WATCOM386_WIN_WIN16)/$(1).rc: $$(OBJDIR_WATCOM386_WIN_WIN16)/$(1).ico
	echo "1000   ICON   \"$$(OBJDIR_WATCOM386_WIN_WIN16)/$(1).ico\"" > $$@

# GCC-Win-Win32

RES_GCC_WIN_WIN32_DEP=$$(OBJDIR_GCC_WIN_WIN32)/$(1).res

$$(OBJDIR_GCC_WIN_WIN32)/$(1).ico: $(1).bmp
	convert "$$<" "$$@"

$$(OBJDIR_GCC_WIN_WIN32)/$(1).rc: $$(OBJDIR_GCC_WIN_WIN32)/$(1).ico
	echo "1000   ICON   \"$$(OBJDIR_GCC_WIN_WIN32)/$(1).ico\"" > $$@

$$(OBJDIR_GCC_WIN_WIN32)/$(1).res: $$(OBJDIR_GCC_WIN_WIN32)/$(1).rc
	$(WINDRES_GCC_MINGW) "$$<" -O coff -o "$$@"

# GCC64-Win-Win32

RES_GCC64_WIN_WIN32_DEP=$$(OBJDIR_GCC64_WIN_WIN32)/$(1).res

$$(OBJDIR_GCC64_WIN_WIN32)/$(1).ico: $(1).bmp
	convert "$$<" "$$@"

$$(OBJDIR_GCC64_WIN_WIN32)/$(1).rc: $$(OBJDIR_GCC64_WIN_WIN32)/$(1).ico
	echo "1000   ICON   \"$$(OBJDIR_GCC64_WIN_WIN32)/$(1).ico\"" > $$@

$$(OBJDIR_GCC64_WIN_WIN32)/$(1).res: $$(OBJDIR_GCC64_WIN_WIN32)/$(1).rc
	$(WINDRES_GCC64_MINGW) "$$<" -O coff -o "$$@"

endef
endif

# ---

define TGT_WATCOM_WIN_WIN32

ifeq ("$$(OPENGL)","1")
	LDFLAGS_WATCOM_WIN_WIN32 += libr opengl32
endif

$(1)nt.exe: \
$$(addprefix $$(OBJDIR_WATCOM_WIN_WIN32)/,$$(subst .c,.o,$$(C_FILES))) | \
$$(RES_WATCOM_WIN_WIN32_DEP)
	$$(LD_WATCOM) name $$@ \
		$$(LDFLAGS_WATCOM_WIN) $$(LDFLAGS_WATCOM_WIN_WIN32) fil {$$^} \
		$$(RES_WATCOM_WIN_WIN32)

$$(OBJDIR_WATCOM_WIN_WIN32)/%.o: %.c | $(2)
	$$(MD) "$$(dir $$@)"
	$$(CC_WATCOM) \
		$$(CFLAGS_WATCOM) $$(CFLAGS_WATCOM_WIN) $$(CFLAGS_WATCOM_WIN_WIN32) \
		-fo=$$@ $$(<:%.c=%)

CLEAN_TARGETS += $(1)nt.exe $(2)

endef

# Legacy
TGTWINNT=$(TGT_WATCOM_WIN_WIN32)

# ---

define TGT_WATCOM386_WIN_WIN16

$(1)w.rex: \
$(addprefix $$(OBJDIR_WATCOM386_WIN_WIN16)/,$(subst .c,.o,$(C_FILES))) | \
$$(OBJDIR_WATCOM386_WIN_WIN16)/$(1).rc
	$$(LD_WATCOM) name $$@ \
		$$(LDFLAGS_WATCOM_WIN) $$(LDFLAGS_WATCOM386_WIN_WIN16) fil {$$^}

$(1)w.exe: $(1)w.rex
	wbind "$$<" -s "$$(WATCOM)/binw/win386.ext" -R \
	$$(OBJDIR_WATCOM386_WIN_WIN16)/$(1).rc "$$@"

$$(OBJDIR_WATCOM386_WIN_WIN16)/%.o: %.c | $(2)
	$$(MD) "$$(dir $$@)"
	$$(CC_WATCOM) \
		$$(CFLAGS_WATCOM) $$(CFLAGS_WATCOM_WIN) $$(CFLAGS_WATCOM386_WIN_WIN16) \
		-fo="$$@" "$$(<:%.c=%)"

CLEAN_TARGETS += $(1)w.exe $(1)w.rex $(2)

endef

# Legacy
TGTWIN386=$(TGT_WATCOM386_WIN_WIN16)

#---

define TGT_GCC_WIN_WIN32

ifeq ("$$(OPENGL)","1")
	LDFLAGS_GCC_WIN_WIN32 += -lopengl32
endif

$(1)ntg.exe: \
$$(addprefix $$(OBJDIR_GCC_WIN_WIN32)/,$$(subst .c,.o,$$(C_FILES))) \
$$(RES_GCC_WIN_WIN32_DEP)
	$$(LD_GCC_MINGW) -o $$@ $$^ $$(LDFLAGS_GCC_WIN) $$(LDFLAGS_GCC_WIN_WIN32)

$$(OBJDIR_GCC_WIN_WIN32)/%.o: %.c | $(2)
	$$(MD) "$$(dir $$@)"
	$$(CC_GCC_MINGW) \
		$$(GLOBAL_DEFINES) $$(INCLUDES_GCC) $$(DEFINES_GCC) \
		$$(CFLAGS_GCC_WIN) $$(CFLAGS_GCC_WIN_WIN32) \
		-o $$@ -c $$<

CLEAN_TARGETS += $(1)ntg.exe $(2)

endef

# Legacy
TGTWINNTGCC=$(TGT_GCC_WIN_WIN32)

#---

define TGT_GCC64_WIN_WIN32

ifeq ("$$(OPENGL)","1")
	LDFLAGS_GCC64_WIN_WIN32 += -lopengl32
endif

$(1)64.exe: \
$$(addprefix $$(OBJDIR_GCC64_WIN_WIN32)/,$$(subst .c,.o,$$(C_FILES))) \
$$(RES_GCC64_WIN_WIN32_DEP)
	$$(LD_GCC_MINGW64) -m64 -o $$@ $$^ \
		$$(LDFLAGS_GCC_WIN) $$(LDFLAGS_GCC64_WIN_WIN32)

$$(OBJDIR_GCC64_WIN_WIN32)/%.o: %.c | $(2)
	$$(MD) "$$(dir $$@)"
	$$(CC_GCC_MINGW64) -m64 \
		$$(GLOBAL_DEFINES) $$(INCLUDES_GCC) $$(DEFINES_GCC) \
		$$(CFLAGS_GCC_WIN) $$(CFLAGS_GCC64_WIN_WIN32) \
		-o $$@ -c $$<

CLEAN_TARGETS += $(1)64.exe $(2)

endef

# Legacy
TGTWIN64GCC=$(TGT_GCC64_WIN_WIN32)

