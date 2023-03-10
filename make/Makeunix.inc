
# vim: ft=make noexpandtab

# Setup default CFLAGS/LDFLAGS for all UNIX targets.
ifeq ("$(CFLAGS_GCC_UNIX)","")
	CFLAGS_GCC_UNIX := -DRETROFLAT_OS_UNIX
endif
ifeq ("$(LDFLAGS_GCC_UNIX)","")
	LDFLAGS_GCC_UNIX :=
endif

ifeq ("$(OPENGL)","1")
   SDL_VER_UNIX := 1
endif

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

ifeq ("$$(SDL_VER_UNIX)","1")
	CFLAGS_GCC_UNIX_SDL := -DRETROFLAT_API_SDL1 $$(shell pkg-config sdl --cflags)
	LDFLAGS_GCC_UNIX_SDL := $$(shell pkg-config sdl --libs)
else
	CFLAGS_GCC_UNIX_SDL := -DRETROFLAT_API_SDL2 $$(shell pkg-config sdl2 --cflags)
	LDFLAGS_GCC_UNIX_SDL := $$(shell pkg-config sdl2 --libs)
endif

ifeq ("$$(OPENGL)","1")
	CFLAGS_GCC_UNIX_SDL += $(shell pkg-config gl --cflags)
	LDFLAGS_GCC_UNIX_SDL += $(shell pkg-config gl --libs)
endif

$(1).sdl: $$(addprefix obj/$$(shell uname -s)-sdl/,$$(subst .c,.o,$$(C_FILES)))
	$$(CC_GCC) -o $$@ $$^ \
		$$(LDFLAGS_GCC) $$(LDFLAGS_GCC_UNIX) $$(LDFLAGS_GCC_UNIX_SDL)

obj/$$(shell uname -s)-sdl/%.o: %.c | $(MAUG_DEPS)
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

CFLAGS_GCC_UNIX_ALLEGRO := -DRETROFLAT_API_ALLEGRO $$(shell pkg-config allegro --cflags)
LDFLAGS_GCC_UNIX_ALLEGRO := $$(shell pkg-config allegro --libs)

$(1).ale: $$(addprefix obj/$$(shell uname -s)-ale/,$$(subst .c,.o,$$(C_FILES)))
	$$(CC_GCC) -o $$@ $$^ \
		$$(LDFLAGS_GCC) $$(LDFLAGS_GCC_UNIX) $$(LDFLAGS_GCC_UNIX_ALLEGRO)

obj/$$(shell uname -s)-ale/%.o: %.c | $(MAUG_DEPS)
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

CFLAGS_GCC_UNIX_GLUT := -DRETROFLAT_API_GLUT $(shell pkg-config gl --cflags)
LDFLAGS_GCC_UNIX_GLUT := -lglut $(shell pkg-config gl --libs)

$(1).glut: $$(addprefix obj/$$(shell uname -s)-glut/,$$(subst .c,.o,$$(C_FILES)))
	$$(CC_GCC) -o $$@ $$^ \
		$$(LDFLAGS_GCC) $$(LDFLAGS_GCC_UNIX) $$(LDFLAGS_GCC_UNIX_GLUT)

obj/$$(shell uname -s)-glut/%.o: %.c | $(MAUG_DEPS)
	$$(MD) $$(dir $$@)
	$$(CC_GCC) -c -o $$@ $$< \
		$$(CFLAGS_GCC) $$(CFLAGS_GCC_UNIX) $$(CFLAGS_GCC_UNIX_GLUT)

CLEAN_TARGETS += $(1).glut

endef

