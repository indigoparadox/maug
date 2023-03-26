
# vim: ft=make noexpandtab

INCLUDES_GCC += -Imaug/src
INCLUDES_WATCOM += -i=maug/src

ifeq ("$(OPENGL)","1")
	DEFINES_GCC += -DRETROFLAT_OPENGL
	DEFINES_WATCOM += -DRETROFLAT_OPENGL
endif

CFLAGS_GCC := $(GLOBAL_DEFINES) $(INCLUDES_GCC) $(DEFINES_GCC)
CFLAGS_GCC64 := $(GLOBAL_DEFINES) $(INCLUDES_GCC) $(DEFINES_GCC)
CFLAGS_WATCOM := $(GLOBAL_DEFINES) $(INCLUDES_WATCOM) $(DEFINES_WATCOM)
CFLAGS_WATCOM386 := $(GLOBAL_DEFINES) $(INCLUDES_WATCOM) $(DEFINES_WATCOM)
LDFLAGS_GCC :=  $(LIBS_GCC)
LDFLAGS_GCC64 :=  $(LIBS_GCC)
LDFLAGS_WATCOM :=  $(LIBS_WATCOM)
LDFLAGS_WATCOM386 :=  $(LIBS_WATCOM)

CLEAN_TARGETS := obj *.err

#DEFINES_GLOBAL = -DLOG_TO_FILE -DLOG_FILE_NAME=\"out.log\"

# Build debug unless release specified.
ifeq ("$(RELEASE)","RELEASE")
	CFLAGS_WATCOM += -ot
	CFLAGS_WATCOM386 += -ot
	CFLAGS_GCC += -O2
	CFLAGS_GCC64 += -O2
else
	CFLAGS_WATCOM += -d3 -DDEBUG_LOG -DDEBUG -DDEBUG_THRESHOLD=1
	CFLAGS_WATCOM386 += -d3 -DDEBUG_LOG -DDEBUG -DDEBUG_THRESHOLD=1
	CFLAGS_GCC += -DDEBUG -Wall -g
	CFLAGS_GCC64 += -DDEBUG -Wall -g
	LDFLAGS_GCC += -g
	LDFLAGS_GCC64 += -g
endif

ifeq ("$(PEDANTIC)","1")
	CFLAGS_GCC += -Wpedantic
	CFLAGS_GCC64 += -Wpedantic
endif

ifeq ("$(WERROR)","1")
	CFLAGS_GCC += -Werror
	CFLAGS_GCC64 += -Werror
	CFLAGS_WATCOM += -we
	CFLAGS_WATCOM386 += -we
endif

ifeq ("$(C89)","1")
	CFLAGS_GCC += -std=c89
	CFLAGS_GCC64 += -std=c89
endif

# Default compiler specs.
ifeq ("$(CC_GCC)","")
	CC_GCC := gcc
endif
ifeq ("$(CC_WATCOM)","")
	CC_WATCOM := wcc
endif
ifeq ("$(LD_WATCOM)","")
	LD_WATCOM := wlink
endif
ifeq ("$(CC_WATCOM386)","")
	CC_WATCOM386 := wcc386
endif
ifeq ("$(LD_WATCOM386)","")
	LD_WATCOM386 := wlink
endif
ifeq ("$(EMCC)","")
	EMCC := emcc
endif
MD := mkdir -p

include maug/make/Makeico.inc
include maug/make/Maketpl.inc
include maug/make/Makexpm.inc
include maug/make/Makeunix.inc
include maug/make/Makewin.inc
include maug/make/Makewasm.inc
include maug/make/Makedos.inc
include maug/make/Makends.inc
include maug/make/Makeos2.inc

