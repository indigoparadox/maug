
# vim: ft=make noexpandtab

define TGTWASMSDL

ifeq ($$(FORCE_MVFS),)
ifeq ($$(MVFS_AVAIL),1)
# Selectively enable VFS for this platform.
$$(info Selectively enabling MVFS for platform "WASM" with no filesystem!)
CFLAGS_OPT_WASM += -D$$(MVFS_DEFINE) -I$$(MVFS_H_DIR)
MAUG_DEPS_WASM += $$(MVFS_H_DIR)/mvfs.h
endif
endif

ifeq ($$(FORCE_XPM),)
ifeq ($$(XPM_AVAIL),1)
# Selectively enable XPM for this platform.
$$(info Selectively enabling XPM for platform "WASM" with no filesystem!)
CFLAGS_OPT_WASM += -DRETROFLAT_XPM -I$$(RETROXPM_H_DIR)
MAUG_DEPS_WASM += $$(RETROXPM_H_DIR)/xpmasset.h
endif
endif

ifneq ("$$(EMCC_CONFIG)","")
	EMCC_EM_CONFIG := --em-config $$(EMCC_CONFIG)
endif

EMCC_FLAGS_SDL += -DRETROFLAT_OS_WASM -DRETROFLAT_NO_CLI \
	-I$(MAUG_ROOT)/api/retro2d/sdl -I$(MAUG_ROOT)/api/input/sdl -s USE_OGG=1

ifeq ("$$(RETROFLAT_SOUND)","1")
	EMCC_FLAGS_SDL += -I$(MAUG_ROOT)/api/sound/sdl
endif

ifneq ("$$(AUDIO_FILES)","")
	EMCC_FLAGS_SDL += --use-preload-plugins $$(addprefix --preload-file ,$$(AUDIO_FILES))
endif

ifeq ("$$(OPENGL)","1")
   SDL_VER_WASM := 1
	EMCC_FLAGS_SDL += -s LEGACY_GL_EMULATION=1 -s GL_UNSAFE_OPTS=1 -DRETROFLAT_OPENGL
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

CLEAN_TARGETS += $(1).html $(1).js $(1).wasm

endef

