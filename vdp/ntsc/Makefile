
# vim: ft=make noexpandtab

MAUG_DIR := ../..

include $(MAUG_DIR)/make/Makevdp.inc

all: rvdpsdl1.so rvdpsdl2.so rvdpnt.dll

$(eval $(call TGT_GCC_UNIX_SDL_VDP,retrovdp.c))

$(eval $(call TGT_WATCOM_WIN_VDP,retrovdp.c))

clean:
	rm -f rvdp*.so rvdp*.o rvdp*.dll

