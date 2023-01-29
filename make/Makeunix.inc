
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

define TGTUNIXSDL

ifeq ("$$(SDL_VER_UNIX)","1")
	CFLAGS_GCC_UNIX_SDL := -DRETROFLAT_API_SDL1 $$(shell pkg-config sdl --cflags)
	LDFLAGS_GCC_UNIX_SDL := $$(shell pkg-config sdl --libs) -lSDL_ttf
else
	CFLAGS_GCC_UNIX_SDL := -DRETROFLAT_API_SDL2 $$(shell pkg-config sdl2 --cflags)
	LDFLAGS_GCC_UNIX_SDL := $$(shell pkg-config sdl2 --libs) -lSDL_ttf
endif

ifeq ("$$(OPENGL)","1")
	CFLAGS_GCC_UNIX_SDL += -DRETROFLAT_OPENGL
	LDFLAGS_GCC_UNIX_SDL += $(shell pkg-config opengl --libs)
endif

$(1).sdl: $$(addprefix obj/$$(shell uname -s)-sdl/,$$(subst .c,.o,$$(C_FILES)))
	$$(CC_GCC) -o $$@ $$^ \
		$$(LDFLAGS_GCC) $$(LDFLAGS_GCC_UNIX) $$(LDFLAGS_GCC_UNIX_SDL)

obj/$$(shell uname -s)-sdl/%.o: %.c
	$$(MD) $$(dir $$@)
	$$(CC_GCC) -c -o $$@ $$< \
		$$(CFLAGS_GCC) $$(CFLAGS_GCC_UNIX) $$(CFLAGS_GCC_UNIX_SDL)

CLEAN_TARGETS += $(1).sdl

endef

# ---

define TGTUNIXALE

CFLAGS_GCC_UNIX_ALLEGRO := -DRETROFLAT_API_ALLEGRO $$(shell pkg-config allegro --cflags)
LDFLAGS_GCC_UNIX_ALLEGRO := $$(shell pkg-config allegro --libs)

$(1).ale: $$(addprefix obj/$$(shell uname -s)-ale/,$$(subst .c,.o,$$(C_FILES)))
	$$(CC_GCC) -o $$@ $$^ \
		$$(LDFLAGS_GCC) $$(LDFLAGS_GCC_UNIX) $$(LDFLAGS_GCC_UNIX_ALLEGRO)

obj/$$(shell uname -s)-ale/%.o: %.c
	$$(MD) $$(dir $$@)
	$$(CC_GCC) -c -o $$@ $$< \
		$$(CFLAGS_GCC) $$(CFLAGS_GCC_UNIX) $$(CFLAGS_GCC_UNIX_ALLEGRO)

CLEAN_TARGETS += $(1).ale

endef

