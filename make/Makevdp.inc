
# vim: ft=make noexpandtab

SO_CFLAGS := -DRETROFLAT_OS_UNIX
SO_LDFLAGS := 

DLL_CFLAGS := -DRETROFLAT_OS_WIN

ifneq ("$(RELEASE)","RELEASE")
SO_CFLAGS += -DDEBUG_THRESHOLD=1 -DDEBUG_LOG -DDEBUG
DLL_CFLAGS += -DDEBUG_THRESHOLD=1 -DDEBUG_LOG -DDEBUG
endif

define TGT_GCC_UNIX_SDL_VDP

rvdpsdl1.so: $(1)
	gcc -I$$(MAUG_DIR)/src -I$$(MAUG_DIR)/api/retro2d/sdl1 \
		-I$$(MAUG_DIR)/api/mem/unix -I$$(MAUG_DIR)/api/file/unix \
		-I$$(MAUG_DIR)/api/log/unix -I$$(MAUG_DIR)/api/input/sdl1 \
		-fpic -shared -o $$@ $$< -DRETROFLAT_API_SDL1 -DRETROFLAT_NO_SOUND \
		`pkg-config --libs --cflags sdl` $$(SO_CFLAGS) $$(SO_LDFLAGS)

rvdpsdl2.so: $(1)
	gcc -I$$(MAUG_DIR)/src -I$$(MAUG_DIR)/api/retro2d/sdl2 \
		-I$$(MAUG_DIR)/api/mem/unix -I$$(MAUG_DIR)/api/file/unix \
		-I$$(MAUG_DIR)/api/log/unix -I$$(MAUG_DIR)/api/input/sdl2 \
		-fpic -shared -o $$@ $$< -DRETROFLAT_API_SDL2 -DRETROFLAT_NO_SOUND \
		`pkg-config --libs --cflags sdl2` $$(SO_CFLAGS) $$(SO_LDFLAGS)

endef

# ---

define TGT_WATCOM_WIN_VDP

rvdpnt.o: $(1)
	wcc386 -I$$(MAUG_DIR)/src -I$$(MAUG_DIR)/api/win \
		-I$$(MAUG_DIR)/api/mem/win -I$$(MAUG_DIR)/api/file/win \
		-I$$(MAUG_DIR)/api/log/unix -I$$(MAUG_DIR)/api/input/win \
		-DRETROFLAT_API_WIN32 -DRETROFLAT_NO_SOUND \
		-I$$(WATCOM)/h/nt -bd $$< -fo=$$@ $$(DLL_CFLAGS) -mf -5r -fp5

rvdpnt.dll: rvdpnt.o
	wlink system nt_dll name rvdpnt file rvdpnt

endef

