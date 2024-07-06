
# vim: ft=make noexpandtab

ifeq ($(MAUG_ROOT),)
	MAUG_ROOT := maug
endif

INCLUDES_GCC += -I$(MAUG_ROOT)/src
INCLUDES_WATCOM += -i=$(MAUG_ROOT)/src

ifeq ("$(OPENGL)","1")
	DEFINES_GCC += -DRETROFLAT_OPENGL
	DEFINES_WATCOM += -DRETROFLAT_OPENGL
endif

ifeq ($(FORCE_MVFS),1)
	DEFINES_GCC += -DMVFS_ENABLED
	DEFINES_WATCOM += -DMVFS_ENABLED
	INCLUDES_GCC += -Iobj/mvfs
	INCLUDES_WATCOM += -i=obj/mvfs
	MAUG_DEPS += obj/mvfs/mvfs.h
endif

ifeq ($(FORCE_RETROXPM),1)
	DEFINES_GCC += -DRETROFLAT_XPM
	DEFINES_WATCOM += -DRETROFLAT_XPM
	INCLUDES_GCC += -Iobj/retroxpm
	INCLUDES_WATCOM += -i=obj/retroxpm
	MAUG_DEPS += obj/retroxpm/xpmasset.h
endif

GLOBAL_DEFINES += -DRETROFLAT_COMMIT_HASH="\"$(shell cd maug && git rev-parse HEAD && cd ..)\""

CFLAGS_GCC := $(GLOBAL_DEFINES) $(INCLUDES_GCC) $(DEFINES_GCC)
CFLAGS_GCC64 := $(GLOBAL_DEFINES) $(INCLUDES_GCC) $(DEFINES_GCC)
CFLAGS_WATCOM := $(GLOBAL_DEFINES) $(INCLUDES_WATCOM) $(DEFINES_WATCOM)
CFLAGS_WATCOM386 := $(GLOBAL_DEFINES) $(INCLUDES_WATCOM) $(DEFINES_WATCOM)
CFLAGS_EMCC := $(GLOBAL_DEFINES) $(INCLUDES_GCC)
LDFLAGS_GCC :=  $(LIBS_GCC)
LDFLAGS_GCC64 :=  $(LIBS_GCC)
LDFLAGS_WATCOM :=  $(LIBS_WATCOM)
LDFLAGS_WATCOM386 :=  $(LIBS_WATCOM)

CLEAN_TARGETS := obj *.err

#DEFINES_GLOBAL = -DLOG_TO_FILE -DLOG_FILE_NAME=\"out.log\"

ifeq ($(DEBUG_THRESHOLD),)
	DEBUG_THRESHOLD := 1
endif

# Build debug unless release specified.
ifeq ("$(RELEASE)","RELEASE")
	CFLAGS_WATCOM += -ot
	CFLAGS_WATCOM386 += -ot
	CFLAGS_GCC += -O2
	CFLAGS_GCC64 += -O2
else
	CFLAGS_WATCOM += -d3 -db -DDEBUG_LOG -DDEBUG -DDEBUG_THRESHOLD=$(DEBUG_THRESHOLD)
	CFLAGS_WATCOM386 += -d3 -db -DDEBUG_LOG -DDEBUG -DDEBUG_THRESHOLD=$(DEBUG_THRESHOLD)
	CFLAGS_GCC += -DDEBUG -Wall -g -pg -DMPARSER_TRACE_NAMES
	CFLAGS_GCC64 += -DDEBUG -Wall -g -pg
	LDFLAGS_GCC += -g -pg
	LDFLAGS_GCC64 += -g -pg
	CFLAGS_EMCC += -DDEBUG -DDEBUG_LOG -DDEBUG_THRESHOLD=$(DEBUG_THRESHOLD) -Wall
	LDFLAGS_EMCC += --profiling-functions
	LDFLAGS_WATCOM386 += debug all
endif

ifeq ("$(PEDANTIC)","1")
	CFLAGS_GCC += -Wpedantic
	CFLAGS_GCC64 += -Wpedantic
endif

ifeq ("$(WERROR)","1")
	CFLAGS_GCC += -Werror -Wsign-compare
	CFLAGS_GCC64 += -Werror -Wsign-compare
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
include maug/make/Makevfs.inc
include maug/make/Makexpm.inc
include maug/make/Makeunix.inc
include maug/make/Makewin.inc
include maug/make/Makewasm.inc
include maug/make/Makedos.inc
include maug/make/Makends.inc
include maug/make/Makeos2.inc
include maug/make/Makeplug.inc

