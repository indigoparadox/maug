divert(-1)
define(`ntexea', `$1nt.exe')
define(`wexea', `$1w.exe')
define(`dexea', `$1d.exe')
divert(0)
`#' vim: ft=make noexpandtab

RETROFLAT_SOUND := 1
RETROFLAT_VDP := 1
RETROFLAT_OPENGL := 0

C_FILES := src/main.c

include maug/Makefile.inc

# Target-specific options.
.PHONY: clean

all: template.ale template.sdl dexea(template) wexea(template) ntexea(template) template.html

# MacOS

$(eval $(call TGTMAC68K,template))

# Unix (Allegro)

$(eval $(call TGTUNIXALE,template))

# Unix (SDL)

$(eval $(call TGTUNIXSDL,template))

# WASM

$(eval $(call TGTWASMSDL,template))

# DOS

$(eval $(call TGTDOSALE,template))

# WinNT

$(eval $(call TGTWINNT,template))

# Win386

$(eval $(call TGTWIN386,template))

# Clean

clean:
	rm -rf $(CLEAN_TARGETS)

