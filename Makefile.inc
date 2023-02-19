
# vim: ft=make noexpandtab

CFLAGS_GCC := $(GLOBAL_DEFINES) -Imaug/src
CFLAGS_WATCOM := $(GLOBAL_DEFINES) -imaug/src
LDFLAGS_GCC := 

CLEAN_TARGETS := obj *.err

#DEFINES_GLOBAL = -DLOG_TO_FILE -DLOG_FILE_NAME=\"out.log\"

# Build debug unless release specified.
ifneq ("$(RELEASE)","RELEASE")
	CFLAGS_WATCOM += -we -d3 -DDEBUG_LOG -DDEBUG -DDEBUG_THRESHOLD=1
	CFLAGS_GCC += -DDEBUG -Wall -g -fsanitize=address -fsanitize=leak -fsanitize=undefined -DDEBUG_LOG -DDEBUG -DDEBUG_THRESHOLD=1
	LDFLAGS_GCC += -g -fsanitize=address -fsanitize=leak -fsanitize=undefined
endif

# Default compiler specs.
ifeq ("$(CC_GCC)","")
	CC_GCC := gcc
endif
ifeq ("$(CC_WATCOM)","")
	CC_WATCOM := wcc386
endif
ifeq ("$(LD_WATCOM)","")
	LD_WATCOM := wlink
endif
ifeq ("$(EMCC)","")
	EMCC := emcc
endif
MD := mkdir -p

include maug/make/Maketpl.inc
include maug/make/Makexpm.inc
include maug/make/Makeunix.inc
include maug/make/Makewin.inc
include maug/make/Makewasm.inc
include maug/make/Makedos.inc
include maug/make/Makeos2.inc

