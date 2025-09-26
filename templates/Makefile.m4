divert(-1)
define(`ntexea', `$1nt.exe')
define(`wexea', `$1w3.exe')
define(`dexea', `$1b.exe')
divert(0)
`#' vim: ft=make noexpandtab

RETROFLAT_SOUND := 1
RETROFLAT_VDP := 1
RETROFLAT_OPENGL := 0

C_FILES := src/main.c

include maug/Makefile.inc

# Target-specific options.
.PHONY: clean

all: template.sdl dexea(template) wexea(template) ntexea(template) template.html

# MacOS

$(eval $(call TGTMAC68K,template))

# Unix (SDL)

$(eval $(call TGTUNIXSDL,template))

# WASM

$(eval $(call TGTWASMSDL,template))

# DOS

$(eval $(call TGTDOSBIOS,template))

# WinNT

$(eval $(call TGTWINNT,template))

# Win16

$(eval $(call TGTWIN16,template))

# Clean

clean:
	rm -rf $(CLEAN_TARGETS)

