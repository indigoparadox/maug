
# vim: ft=make noexpandtab

# Setup default CFLAGS/LDFLAGS for all UNIX targets.
ifeq ("$(CFLAGS_GCC_UNIX)","")
	CFLAGS_GCC_UNIX := -DRETROFLAT_OS_UNIX -DRETROSND_ARGS
endif
ifeq ("$(LDFLAGS_GCC_UNIX)","")
	LDFLAGS_GCC_UNIX := -ldl
endif
ifeq ("$(CFLAGS_GCC64_UNIX)","")
	CFLAGS_GCC64_UNIX := -DRETROFLAT_OS_UNIX -DRETROSND_ARGS
endif
ifeq ("$(LDFLAGS_GCC64_UNIX)","")
	LDFLAGS_GCC64_UNIX := -ldl
endif

ifneq ("$(BUILD)","RELEASE")
	# Build debug unless release specified.
	GCC_FSAN := -fsanitize=address -fsanitize=leak -fsanitize=undefined 
	CFLAGS_GCC_UNIX += $(GCC_FSAN) -DDEBUG_LOG -DDEBUG -DDEBUG_THRESHOLD=$(DEBUG_THRESHOLD)
	LDFLAGS_GCC_UNIX += $(GCC_FSAN)
	CFLAGS_GCC64_UNIX += $(GCC_FSAN) -DDEBUG_LOG -DDEBUG -DDEBUG_THRESHOLD=$(DEBUG_THRESHOLD)
	LDFLAGS_GCC64_UNIX += $(GCC_FSAN)
endif

ifeq ("$(RETROFLAT_OPENGL)","1")

# Easier to understand error messages.
ifeq ("$(shell pkg-config gl --libs 2>/dev/null)","")
$(error could not find pkg-config for gl!)
endif

CFLAGS_GCC_UNIX += -I$(MAUG_ROOT)/api/retro3d/opengl
CFLAGS_GCC64_UNIX += -I$(MAUG_ROOT)/api/retro3d/opengl
CFLAGS_GCC_UNIX += $(shell pkg-config gl --cflags)
LDFLAGS_GCC_UNIX += $(shell pkg-config gl --libs)
CFLAGS_GCC64_UNIX += $(shell pkg-config gl --cflags)
LDFLAGS_GCC64_UNIX += $(shell pkg-config gl --libs)

endif
ifeq ("$(SMALL3DL)","1")

CFLAGS_GCC_UNIX += -I$(MAUG_ROOT)/api/retro3d/small3dl
CFLAGS_GCC64_UNIX += -I$(MAUG_ROOT)/api/retro3d/small3dl

endif

OBJDIR_GCC_UNIX_ALLEGRO=obj/gcc-$(shell uname -s)-allegro
OBJDIR_GCC_UNIX_GLUT=obj/gcc-$(shell uname -s)-glut
OBJDIR_GCC_UNIX_XLIB=obj/gcc-$(shell uname -s)-xlib

# ---

# General utility macro to be called below to pick up the application icon
# (<appname>.bmp) *or* the default maug icon if no application icon present.

define TGT_ICO_UNIX_SDL

$(2)/ico/sdl_ico.bmp:
	$$(MD) "$$(dir $$@)"
	if [ -f "$(1).bmp" ]; then cp "$(1).bmp" "$$@"; else cp "$$(MAUG_ROOT)/maug16.bmp" "$$@"; fi

ifeq ($$(XXD),)
XXD := $$(shell which xxd)
ifeq ($$(XXD),)
$$(error xxd is not installed!)
endif
endif

$(2)/ico/sdl_ico.h: $(2)/ico/sdl_ico.bmp
	$$(XXD) -i $$< | sed 's/obj_.*_sdl/obj_ico_sdl/g' > $$@

endef

# ---

# Target: UNIX OS/SDL API
# Variables:
# - SDL_VER_UNIX
# - RETROFLAT_OPENGL
# Parameters:
# 1. Target name. Will have ".sdl" appended to it.
# 2. Source dependencies. Used for e.g. dynamically generated headers.
# Only #1 will be added to CLEAN_TARGETS!
define TGTUNIXSDL

# = Determine Prerequisites =

# Set this before OBJDIR_GCC_UNIX_SDL is set below.
ifeq ("$$(RETROFLAT_VDP)","1")
	CFLAGS_GCC_UNIX_SDL += -DRETROFLAT_VDP
	#LDFLAGS_GCC_UNIX_SDL += -ldl
	SDL_VER_UNIX := 1
endif

ifeq ("$$(RETROFLAT_SOUND)","1")
	CFLAGS_GCC_UNIX_SDL += -I$(MAUG_ROOT)/api/sound/sdl
endif

# Set this before OBJDIR_GCC_UNIX_SDL is set below.
ifeq ("$$(SDL_VER_UNIX)","1")

# Easier to understand error messages.
ifeq ("$$(shell pkg-config sdl --libs 2>/dev/null)","")
$$(error could not find pkg-config for SDL 1.2!)
endif
ifeq ("$$(shell pkg-config SDL_mixer --libs 2>/dev/null)","")
$$(error could not find pkg-config for SDL_mixer 1.2!)
endif

# Actually setup flags.
CFLAGS_GCC_UNIX_SDL += \
	-DRETROFLAT_API_SDL1 $$(shell pkg-config sdl SDL_mixer --cflags) -DRETROSND_API_SDL1
LDFLAGS_GCC_UNIX_SDL += $$(shell pkg-config sdl SDL_mixer --libs)

else

# Easier to understand error messages.
ifeq ("$$(shell pkg-config sdl --libs 2>/dev/null)","")
$$(error could not find pkg-config for SDL 2!)
endif
ifeq ("$$(shell pkg-config SDL_mixer --libs 2>/dev/null)","")
$$(error could not find pkg-config for SDL_mixer 2!)
endif

# Actually setup flags.
CFLAGS_GCC_UNIX_SDL += \
	-DRETROFLAT_API_SDL2 $$(shell pkg-config sdl2 SDL2_mixer --cflags) -DRETROSND_API_SDL2
LDFLAGS_GCC_UNIX_SDL += $$(shell pkg-config sdl2 SDL2_mixer --libs)

# Set a default so this isn't empty.
SDL_VER_UNIX := 2
endif

OBJDIR_GCC_UNIX_SDL=obj/gcc-$$(shell uname -s)-sdl$$(SDL_VER_UNIX)
CFLAGS_GCC_UNIX_SDL += -I$$(MAUG_ROOT)/api/retro2d/sdl \
	-I$(MAUG_ROOT)/api/input/sdl -I$(MAUG_ROOT)/api/file/unix \
	-I$(MAUG_ROOT)/api/font/soft -I$(MAUG_ROOT)/api/mem/unix
CFLAGS_GCC_UNIX_SDL += -DRETROFLAT_SDL_ICO -I$$(OBJDIR_GCC_UNIX_SDL)/ico

ifeq ("$$(RETROFLAT_FORCE_LOG_MFILE)", "1")
CFLAGS_GCC_UNIX_SDL += -I$(MAUG_ROOT)/api/log/mfile
else
CFLAGS_GCC_UNIX_SDL += -I$(MAUG_ROOT)/api/log/unix
endif

# = Make Icons =

$$(eval $$(call TGT_ICO_UNIX_SDL,$(1),$$(OBJDIR_GCC_UNIX_SDL)))

# = Actual Build Targets =

$(1).sdl: $$(addprefix $$(OBJDIR_GCC_UNIX_SDL)/,$$(subst .c,.o,$$(C_FILES)))
	$$(CC_GCC) -o $$@ $$^ \
		$$(LDFLAGS_GCC) $$(LDFLAGS_GCC_UNIX) $$(LDFLAGS_GCC_UNIX_SDL)

$$(OBJDIR_GCC_UNIX_SDL)/%.o: %.c | $$(MAUG_DEPS) $$(OBJDIR_GCC_UNIX_SDL)/ico/sdl_ico.h
	$$(MD) $$(dir $$@)
	$$(CC_GCC) -c -o $$@ $$< \
		$$(CFLAGS_GCC) $$(CFLAGS_GCC_UNIX) $$(CFLAGS_GCC_UNIX_SDL) \
		$$(CFLAGS_OPT_GCC)

CLEAN_TARGETS += $(1).sdl

endef

# ---

# Target: UNIX OS/Allegro API
# Variables:
# Parameters:
# 1. Target name. Will have ".sdl" appended to it.
# 2. Source dependencies. Used for e.g. dynamically generated headers.
# Only #1 will be added to CLEAN_TARGETS!
define TGTUNIXALE

# Easier to understand error messages.
ifeq ("$$(shell pkg-config allegro --libs 2>/dev/null)","")
$$(error could not find pkg-config for allegro!)
endif

# TODO: Use allegro font API.

CFLAGS_GCC_UNIX_ALLEGRO := \
	-DRETROFLAT_API_ALLEGRO $$(shell pkg-config allegro --cflags) \
	-I$(MAUG_ROOT)/api/retro2d/allegro -I$(MAUG_ROOT)/api/input/allegro \
	-I$(MAUG_ROOT)/api/file/unix \
	-I$(MAUG_ROOT)/api/font/soft -I$(MAUG_ROOT)/api/mem/unix
LDFLAGS_GCC_UNIX_ALLEGRO := \
	$$(shell pkg-config allegro --libs)

ifeq ("$$(RETROFLAT_FORCE_LOG_MFILE)", "1")
CFLAGS_GCC_UNIX_ALLEGRO += -I$(MAUG_ROOT)/api/log/mfile
else
CFLAGS_GCC_UNIX_ALLEGRO += -I$(MAUG_ROOT)/api/log/unix
endif

ifeq ("$$(RETROFLAT_SOUND)","1")
	CFLAGS_GCC_UNIX_ALLEGRO += -I$(MAUG_ROOT)/api/sound/null
endif

$(1).ale: \
$$(addprefix $$(OBJDIR_GCC_UNIX_ALLEGRO)/,$$(subst .c,.o,$$(C_FILES)))
	$$(CC_GCC) -o $$@ $$^ \
		$$(LDFLAGS_GCC) $$(LDFLAGS_GCC_UNIX) $$(LDFLAGS_GCC_UNIX_ALLEGRO)

$$(OBJDIR_GCC_UNIX_ALLEGRO)/%.o: %.c | $$(MAUG_DEPS)
	$$(MD) $$(dir $$@)
	$$(CC_GCC) -c -o $$@ $$< \
		$$(CFLAGS_GCC) $$(CFLAGS_GCC_UNIX) $$(CFLAGS_GCC_UNIX_ALLEGRO) \
		$$(CFLAGS_OPT_GCC)

CLEAN_TARGETS += $(1).ale

endef

# ---

# Target: UNIX OS/GLUT API
# Variables:
# Parameters:
# 1. Target name. Will have ".sdl" appended to it.
# 2. Source dependencies. Used for e.g. dynamically generated headers.
# Only #1 will be added to CLEAN_TARGETS!
define TGTUNIXGLUT

# Easier to understand error messages.
ifeq ("$$(shell pkg-config gl --libs 2>/dev/null)","")
$$(error could not find pkg-config for gl!)
endif

CFLAGS_GCC_UNIX_GLUT := -DRETROFLAT_API_GLUT $(shell pkg-config gl --cflags) \
	-I$(MAUG_ROOT)/api/retro2d/glut -I$(MAUG_ROOT)/api/input/glut \
	-I$(MAUG_ROOT)/api/file/unix -DRETROSND_API_ALSA \
	-I$(MAUG_ROOT)/api/font/soft -I$(MAUG_ROOT)/api/mem/unix

LDFLAGS_GCC_UNIX_GLUT := -lglut $(shell pkg-config gl --libs)

ifeq ("$$(RETROFLAT_FORCE_LOG_MFILE)", "1")
CFLAGS_GCC_UNIX_GLUT += -I$(MAUG_ROOT)/api/log/mfile
else
CFLAGS_GCC_UNIX_GLUT += -I$(MAUG_ROOT)/api/log/unix
endif

ifeq ("$$(RETROFLAT_SOUND)","1")
	CFLAGS_GCC_UNIX_GLUT += -I$(MAUG_ROOT)/api/sound/null
	CFLAGS_GCC_UNIX_GLUT += -lasound
endif

$(1).glut: $$(addprefix $$(OBJDIR_GCC_UNIX_GLUT)/,$$(subst .c,.o,$$(C_FILES)))
	$$(CC_GCC) -o $$@ $$^ \
		$$(LDFLAGS_GCC) $$(LDFLAGS_GCC_UNIX) $$(LDFLAGS_GCC_UNIX_GLUT)

$$(OBJDIR_GCC_UNIX_GLUT)/%.o: %.c | $$(MAUG_DEPS)
	$$(MD) $$(dir $$@)
	$$(CC_GCC) -c -o $$@ $$< \
		$$(CFLAGS_GCC) $$(CFLAGS_GCC_UNIX) $$(CFLAGS_GCC_UNIX_GLUT) \
		$$(CFLAGS_OPT_GCC)

CLEAN_TARGETS += $(1).glut

endef

# ---

# Target: UNIX xlib API
# Variables:
# Parameters:
# 1. Target name. Will have ".xlib" appended to it.
# 2. Source dependencies. Used for e.g. dynamically generated headers.
# Only #1 will be added to CLEAN_TARGETS!
define TGTUNIXXLIB

# Easier to understand error messages.
ifeq ("$$(shell pkg-config x11 --libs 2>/dev/null)","")
$$(error could not find pkg-config for x11!)
endif

CFLAGS_GCC_UNIX_XLIB := -DRETROFLAT_API_XLIB $(shell pkg-config x11 --cflags) \
	-I$(MAUG_ROOT)/api/retro2d/xlib -I$(MAUG_ROOT)/api/input/xlib \
	-I$(MAUG_ROOT)/api/file/unix -DRETROSND_API_ALSA \
	-I$(MAUG_ROOT)/api/font/soft -I$(MAUG_ROOT)/api/mem/unix

LDFLAGS_GCC_UNIX_XLIB := $(shell pkg-config x11 --libs)

ifeq ("$$(RETROFLAT_FORCE_LOG_MFILE)", "1")
CFLAGS_GCC_UNIX_XLIB += -I$(MAUG_ROOT)/api/log/mfile
else
CFLAGS_GCC_UNIX_XLIB += -I$(MAUG_ROOT)/api/log/unix
endif

ifeq ("$$(RETROFLAT_SOUND)","1")
	CFLAGS_GCC_UNIX_XLIB += -I$(MAUG_ROOT)/api/sound/null
	LDFLAGS_GCC_UNIX_XLIB += -lasound
endif

$(1).xlib: $$(addprefix $$(OBJDIR_GCC_UNIX_XLIB)/,$$(subst .c,.o,$$(C_FILES)))
	$$(CC_GCC) -o $$@ $$^ \
		$$(LDFLAGS_GCC) $$(LDFLAGS_GCC_UNIX) $$(LDFLAGS_GCC_UNIX_XLIB)

$$(OBJDIR_GCC_UNIX_XLIB)/%.o: %.c | $$(MAUG_DEPS)
	$$(MD) $$(dir $$@)
	$$(CC_GCC) -c -o $$@ $$< \
		$$(CFLAGS_GCC) $$(CFLAGS_GCC_UNIX) $$(CFLAGS_GCC_UNIX_XLIB) \
		$$(CFLAGS_OPT_GCC)

CLEAN_TARGETS += $(1).xlib

endef

