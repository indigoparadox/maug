
# vim: ft=make noexpandtab

define TGTWASMSDL

EMCC_FLAGS_SDL := -DRETROFLAT_OS_WASM -Imaug/src

ifeq ("$$(OPENGL)","1")
   WASM_SDL_VER := 1
	EMCC_FLAGS_SDL += -s LEGACY_GL_EMULATION=1 -s GL_UNSAFE_OPTS=1 -DRETROFLAT_OPENGL
endif

ifeq ("$$(WASM_SDL_VER)","1")
	EMCC_FLAGS_SDL += -DRETROFLAT_API_SDL1 -s USE_SDL=1 -s USE_SDL_TTF=1
else
	EMCC_FLAGS_SDL += -DRETROFLAT_API_SDL2 -s USE_SDL=2 -s USE_SDL_TTF=2
endif

$(1).html: $$(addprefix obj/wasm/,$$(subst .c,.o,$$(C_FILES)))
	$$(EMCC) -o $$@ $$^ $$(EMCC_FLAGS_SDL)

obj/wasm/%.o: %.c | $(MAUG_DEPS)
	$$(MD) $$(dir $$@)
	$$(EMCC) -c -o $$@ $$< $$(EMCC_FLAGS_SDL)

CLEAN_TARGETS += $(1).html $(1).js $(1).wasm

endef

