
# vim: ft=make noexpandtab

define TGTUNIXSDL

ifeq ("$$(OPENGL)","1")
   SDL_VER := 1
endif

ifeq ("$$(SDL_VER)","1")
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
	$$(CC_GCC) -o $$@ $$^ $$(LDFLAGS_GCC) $$(LDFLAGS_GCC_UNIX_SDL)

obj/$$(shell uname -s)-sdl/%.o: %.c
	$$(MD) $$(dir $$@)
	$$(CC_GCC) -c -o $$@ $$< -DRETROFLAT_OS_UNIX $$(CFLAGS_GCC) $$(CFLAGS_GCC_UNIX_SDL)

endef

