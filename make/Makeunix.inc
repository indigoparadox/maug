
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

ifeq ("$(OPENGL)","1")
	CFLAGS_GCC_UNIX += $(shell pkg-config gl --cflags)
	LDFLAGS_GCC_UNIX += $(shell pkg-config gl --libs)
	CFLAGS_GCC64_UNIX += $(shell pkg-config gl --cflags)
	LDFLAGS_GCC64_UNIX += $(shell pkg-config gl --libs)
endif

OBJDIR_GCC_UNIX_SDL=obj/gcc-$(shell uname -s)-sdl$(SDL_VER_UNIX)
OBJDIR_GCC_UNIX_ALLEGRO=obj/gcc-$(shell uname -s)-allegro
OBJDIR_GCC_UNIX_GLUT=obj/gcc-$(shell uname -s)-glut

# ---

# General utility macro to be called below to pick up the application icon
# (<appname>.bmp) *or* the default maug icon if no application icon present.

define TGT_ICO_UNIX_SDL

$(2)/ico/sdl_ico.bmp:
	$$(MD) "$$(dir $$@)"
	if [ -f "$(1).bmp" ]; then cp "$(1).bmp" "$$@"; else cp "$$(MAUG_ROOT)/maug16.bmp" "$$@"; fi

$(2)/ico/sdl_ico.h: $(2)/ico/sdl_ico.bmp
	xxd -i $$< | sed 's/obj_.*_sdl/obj_ico_sdl/g' > $$@

endef

# ---

# Target: UNIX OS/SDL API
# Variables:
# - SDL_VER_UNIX
# - OPENGL
# Parameters:
# 1. Target name. Will have ".sdl" appended to it.
# 2. Source dependencies. Used for e.g. dynamically generated headers.
# Only #1 will be added to CLEAN_TARGETS!
define TGTUNIXSDL

ifeq ("$(CFLAGS_GCC_UNIX_SDL)","")
	CFLAGS_GCC_UNIX_SDL := -DRETROFLAT_SDL_ICO -I$(OBJDIR_GCC_UNIX_SDL)/ico
endif

CFLAGS_GCC_UNIX_SDL += -I$(MAUG_ROOT)/api/sdl

ifeq ("$$(VDP)","1")
	CFLAGS_GCC_UNIX_SDL += -DRETROFLAT_VDP
	#LDFLAGS_GCC_UNIX_SDL += -ldl
	SDL_VER_UNIX := 1
endif

ifeq ("$$(SDL_VER_UNIX)","1")
	CFLAGS_GCC_UNIX_SDL += \
		-DRETROFLAT_API_SDL1 $$(shell pkg-config sdl SDL_mixer --cflags) -DRETROSND_API_SDL1
	LDFLAGS_GCC_UNIX_SDL += $$(shell pkg-config sdl SDL_mixer --libs)
else
	CFLAGS_GCC_UNIX_SDL += \
		-DRETROFLAT_API_SDL2 $$(shell pkg-config sdl2 SDL2_mixer --cflags) -DRETROSND_API_SDL2
	LDFLAGS_GCC_UNIX_SDL += $$(shell pkg-config sdl2 SDL2_mixer --libs)
endif

$(eval $(call TGT_ICO_UNIX_SDL,$(1),$$(OBJDIR_GCC_UNIX_SDL)))

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

CFLAGS_GCC_UNIX_ALLEGRO := \
	-DRETROFLAT_API_ALLEGRO $$(shell pkg-config allegro --cflags) -I$(MAUG_ROOT)/api/allegro
LDFLAGS_GCC_UNIX_ALLEGRO := \
	$$(shell pkg-config allegro --libs)

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

CFLAGS_GCC_UNIX_GLUT := -DRETROFLAT_API_GLUT $(shell pkg-config gl --cflags) -I$(MAUG_ROOT)/api/glut -DRETROSND_API_ALSA

LDFLAGS_GCC_UNIX_GLUT := -lglut $(shell pkg-config gl --libs) -lasound

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

