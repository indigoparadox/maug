
# vim: ft=make noexpandtab

# Setup default CFLAGS/LDFLAGS for all UNIX targets.
ifeq ("$(CFLAGS_GCC_UNIX)","")
	CFLAGS_GCC_UNIX := -DRETROFLAT_OS_UNIX -DRETROSND_API_ALSA -DRETROSND_ARGS
endif
ifeq ("$(LDFLAGS_GCC_UNIX)","")
	LDFLAGS_GCC_UNIX := -lasound -ldl
endif
ifeq ("$(CFLAGS_GCC64_UNIX)","")
	CFLAGS_GCC64_UNIX := -DRETROFLAT_OS_UNIX -DRETROSND_API_ALSA -DRETROSND_ARGS
endif
ifeq ("$(LDFLAGS_GCC64_UNIX)","")
	LDFLAGS_GCC64_UNIX := -lasound -ldl
endif

ifneq ("$(RELEASE)","RELEASE")
	# Build debug unless release specified.
	GCC_FSAN := -fsanitize=address -fsanitize=leak -fsanitize=undefined 
	CFLAGS_GCC_UNIX += $(GCC_FSAN) -DDEBUG_LOG -DDEBUG -DDEBUG_THRESHOLD=$(DEBUG_THRESHOLD)
	LDFLAGS_GCC_UNIX += $(GCC_FSAN)
	CFLAGS_GCC64_UNIX += $(GCC_FSAN) -DDEBUG_LOG -DDEBUG -DDEBUG_THRESHOLD=$(DEBUG_THRESHOLD)
	LDFLAGS_GCC64_UNIX += $(GCC_FSAN)
endif

ifeq ("$(OPENGL)","1")
   SDL_VER_UNIX := 1
	CFLAGS_GCC_UNIX += $(shell pkg-config gl --cflags)
	LDFLAGS_GCC_UNIX += $(shell pkg-config gl --libs)
	CFLAGS_GCC64_UNIX += $(shell pkg-config gl --cflags)
	LDFLAGS_GCC64_UNIX += $(shell pkg-config gl --libs)
else
	ifeq ("$(SDL_VER_UNIX)","")
		SDL_VER_UNIX := 2
	endif
endif

OBJDIR_GCC_UNIX_SDL=obj/gcc-$(shell uname -s)-sdl$(SDL_VER_UNIX)
OBJDIR_GCC_UNIX_ALLEGRO=obj/gcc-$(shell uname -s)-allegro
OBJDIR_GCC_UNIX_GLUT=obj/gcc-$(shell uname -s)-glut

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

CFLAGS_GCC_UNIX_SDL += -I$(MAUG_ROOT)/api/sdl

ifeq ("$$(VDP)","1")
	CFLAGS_GCC_UNIX_SDL += -DRETROFLAT_VDP
	#LDFLAGS_GCC_UNIX_SDL += -ldl
	SDL_VER_UNIX := 1
endif

ifeq ("$$(SDL_VER_UNIX)","1")
	CFLAGS_GCC_UNIX_SDL += \
		-DRETROFLAT_API_SDL1 $$(shell pkg-config sdl --cflags)
	LDFLAGS_GCC_UNIX_SDL += $$(shell pkg-config sdl --libs)
else
	CFLAGS_GCC_UNIX_SDL += \
		-DRETROFLAT_API_SDL2 $$(shell pkg-config sdl2 --cflags)
	LDFLAGS_GCC_UNIX_SDL += $$(shell pkg-config sdl2 --libs)
endif

$(1).sdl: $$(addprefix $$(OBJDIR_GCC_UNIX_SDL)/,$$(subst .c,.o,$$(C_FILES)))
	$$(CC_GCC) -o $$@ $$^ \
		$$(LDFLAGS_GCC) $$(LDFLAGS_GCC_UNIX) $$(LDFLAGS_GCC_UNIX_SDL)

$$(OBJDIR_GCC_UNIX_SDL)/%.o: %.c | $(MAUG_DEPS)
	$$(MD) $$(dir $$@)
	$$(CC_GCC) -c -o $$@ $$< \
		$$(CFLAGS_GCC) $$(CFLAGS_GCC_UNIX) $$(CFLAGS_GCC_UNIX_SDL)

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

$$(OBJDIR_GCC_UNIX_ALLEGRO)/%.o: %.c | $(MAUG_DEPS)
	$$(MD) $$(dir $$@)
	$$(CC_GCC) -c -o $$@ $$< \
		$$(CFLAGS_GCC) $$(CFLAGS_GCC_UNIX) $$(CFLAGS_GCC_UNIX_ALLEGRO)

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

CFLAGS_GCC_UNIX_GLUT := -DRETROFLAT_API_GLUT $(shell pkg-config gl --cflags) -I$(MAUG_ROOT)/api/glut

LDFLAGS_GCC_UNIX_GLUT := -lglut $(shell pkg-config gl --libs)

$(1).glut: $$(addprefix $$(OBJDIR_GCC_UNIX_GLUT)/,$$(subst .c,.o,$$(C_FILES)))
	$$(CC_GCC) -o $$@ $$^ \
		$$(LDFLAGS_GCC) $$(LDFLAGS_GCC_UNIX) $$(LDFLAGS_GCC_UNIX_GLUT)

$$(OBJDIR_GCC_UNIX_GLUT)/%.o: %.c | $(MAUG_DEPS)
	$$(MD) $$(dir $$@)
	$$(CC_GCC) -c -o $$@ $$< \
		$$(CFLAGS_GCC) $$(CFLAGS_GCC_UNIX) $$(CFLAGS_GCC_UNIX_GLUT)

CLEAN_TARGETS += $(1).glut

endef

