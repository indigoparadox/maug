
# vim: ft=make noexpandtab

SO_CFLAGS := -DRETROFLAT_OS_UNIX -DRETROFLAT_API_SDL1 `pkg-config --cflags sdl`
SO_LDFLAGS := `pkg-config --libs sdl`

DLL_CFLAGS := -DRETROFLAT_OS_WIN -DRETROFLAT_API_WIN32

ifneq ("$(RELEASE)","RELEASE")
SO_CFLAGS += -DDEBUG_THRESHOLD=1 -DDEBUG_LOG -DDEBUG
DLL_CFLAGS += -DDEBUG_THRESHOLD=1 -DDEBUG_LOG -DDEBUG
endif

all: retrovdp.so retrovdp.dll

retrovdp.so: retrovdp.c
	gcc -I../maug/src -fpic -shared -o $@ $< $(SO_CFLAGS) $(SO_LDFLAGS)

retrovdp.o: retrovdp.c
	wcc386 -I../maug/src -I$(WATCOM)/h/nt -bd $< -fo=$@ $(DLL_CFLAGS) -mf -5r -fp5

retrovdp.dll: retrovdp.o
	wlink system nt_dll name retrovdp file retrovdp

clean:
	rm -f retrovdp.so retrovdp.o retrovdp.dll

