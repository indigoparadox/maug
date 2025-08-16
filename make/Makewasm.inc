
# vim: ft=make noexpandtab

define TGTWASMSDL

ifneq ("$$(EMCC_CONFIG)","")
	EMCC_EM_CONFIG := --em-config $$(EMCC_CONFIG)
endif

EMCC_FLAGS_SDL += -DRETROFLAT_OS_WASM -DRETROFLAT_NO_CLI \
	-I$(MAUG_ROOT)/api/retro2d/sdl -I$(MAUG_ROOT)/api/input/sdl -s USE_OGG=1 \
	-I$(MAUG_ROOT)/api/log/unix -I$(MAUG_ROOT)/api/font/soft \
	-I$(MAUG_ROOT)/api/mem/unix

# -O2 is needed for HTTP fetch to work!
# TODO: Is MVFS feasible or desirable?
CFLAGS_EMCC += -I$(MAUG_ROOT)/api/file/unix -O2

# These facilitate async wrappers for the file API.
LDFLAGS_EMCC += -s ASYNCIFY -s ASYNCIFY_IMPORTS=[mfile_wasm_fetch] -O2
# NOTE: The -O2 here facilitates ASYNCIFY. It is necessary, or we'll get lots
#       of "unreachable executed" errors at runtime.

ifeq ("$$(RETROFLAT_SOUND)","1")
	EMCC_FLAGS_SDL += -I$(MAUG_ROOT)/api/sound/sdl
endif

# TODO: Load audio files with HTTP fetch?
ifneq ("$$(AUDIO_FILES)","")
	EMCC_FLAGS_SDL += --use-preload-plugins $$(addprefix --preload-file ,$$(AUDIO_FILES))
endif

ifeq ("$$(RETROFLAT_OPENGL)","1")
   SDL_VER_WASM := 1
	EMCC_FLAGS_SDL += -s LEGACY_GL_EMULATION=1 -s GL_UNSAFE_OPTS=1 -I$(MAUG_ROOT)/api/retro3d/opengl -DRETRO3D_NO_NORMALS
endif

ifeq ("$$(SDL_VER_WASM)","1")
	EMCC_FLAGS_SDL += -DRETROFLAT_API_SDL1 -s USE_SDL=1 -s USE_SDL_MIXER=1 -DRETROSND_API_SDL1
else
	EMCC_FLAGS_SDL += -DRETROFLAT_API_SDL2 -s USE_SDL=2 -s USE_SDL_MIXER=2 -DRETROSND_API_SDL2
endif

$(1).html: $$(addprefix obj/wasm/,$$(subst .c,.o,$$(C_FILES)))
	$$(EMCC) $$(EMCC_EM_CONFIG) -o $$@ $$^ $$(LDFLAGS_EMCC) $$(EMCC_FLAGS_SDL)

obj/wasm/%.o: %.c | $$(MAUG_DEPS) $$(MAUG_DEPS_WASM)
	$$(MD) $$(dir $$@)
	$$(EMCC) $$(EMCC_EM_CONFIG) \
		-c -o $$@ $$< $$(CFLAGS_EMCC) $$(EMCC_FLAGS_SDL) \
		$$(CFLAGS_OPT_GCC) $$(CFLAGS_OPT_WASM)

CLEAN_TARGETS += $(1).html $(1).js $(1).wasm $(1).wasm.zip

endef

