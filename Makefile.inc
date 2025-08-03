
# vim: ft=make noexpandtab

ifeq ($(MAUG_ROOT),)
	MAUG_ROOT := maug
endif

INCLUDES_GCC += -I$(MAUG_ROOT)/src
INCLUDES_WATCOM += -i=$(MAUG_ROOT)/src
INCLUDES_VS += /I $(MAUG_ROOT)/src

ifeq ("$(RETROFLAT_OPENGL)","1")
	DEFINES_GCC += -DRETROFLAT_OPENGL -DRETROFLAT_3D
	DEFINES_WATCOM += -DRETROFLAT_OPENGL -DRETROFLAT_3D
endif
ifeq ("$(RETROFLAT_SMALL3DL)","1")
	DEFINES_GCC += -DRETROFLAT_SMALL3DL -DRETROFLAT_3D
	DEFINES_WATCOM += -DRETROFLAT_SMALL3DL -DRETROFLAT_3D
endif

ifneq ("$(RETROFLAT_SOUND)","1")
	DEFINES_GCC += -DRETROFLAT_NO_SOUND
	DEFINES_WATCOM += -DRETROFLAT_NO_SOUND
endif

GLOBAL_DEFINES += -DRETROFLAT_COMMIT_HASH="\"$(shell git -C $(MAUG_ROOT) rev-parse HEAD)\""

CFLAGS_GCC := $(GLOBAL_DEFINES) $(INCLUDES_GCC) $(DEFINES_GCC)
CFLAGS_GCC64 := $(GLOBAL_DEFINES) $(INCLUDES_GCC) $(DEFINES_GCC)
CFLAGS_WATCOM := $(GLOBAL_DEFINES) $(INCLUDES_WATCOM) $(DEFINES_WATCOM)
CFLAGS_WATCOM386 := $(GLOBAL_DEFINES) $(INCLUDES_WATCOM) $(DEFINES_WATCOM)
CFLAGS_EMCC := $(GLOBAL_DEFINES) $(DEFINES_GCC) $(INCLUDES_GCC)
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
ifeq ("$(BUILD)","RELEASE")
	CFLAGS_WATCOM += -ot
	CFLAGS_WATCOM386 += -ot
	CFLAGS_GCC += -O2
	CFLAGS_GCC64 += -O2
	CFLAGS_CECL += /O2 /D "NDEBUG" /W1 /ML
else
	CFLAGS_WATCOM += -d3 -db -DDEBUG_LOG -DDEBUG -DDEBUG_THRESHOLD=$(DEBUG_THRESHOLD)
	CFLAGS_WATCOM386 += -d3 -db -DDEBUG_LOG -DDEBUG -DDEBUG_THRESHOLD=$(DEBUG_THRESHOLD)
	CFLAGS_GCC += -DDEBUG -Wall -g -pg
	CFLAGS_GCC64 += -DDEBUG -Wall -g -pg
	LDFLAGS_GCC += -g -pg
	LDFLAGS_GCC64 += -g -pg
	CFLAGS_EMCC += -g -DDEBUG -DDEBUG_LOG -DDEBUG_THRESHOLD=$(DEBUG_THRESHOLD) -Wall
	LDFLAGS_EMCC += -g --profiling-functions
	LDFLAGS_WATCOM386 += debug all
	CFLAGS_CECL += /W1 /MLd
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

include maug/make/Maketpl.inc
include maug/make/Makevfs.inc
include maug/make/Makexpm.inc
include maug/make/Makeunix.inc
include maug/make/Makewin.inc
include maug/make/Makece.inc
include maug/make/Makewasm.inc
include maug/make/Makedos.inc
include maug/make/Makends.inc
include maug/make/Makeos2.inc
include maug/make/Makemac.inc
include maug/make/Makeplug.inc

# ---

define TGTISO

# Easier to understand error messages.
ifeq ("$$(shell which mkisofs 2>/dev/null)","")
$$(error could not find mkisofs!)
endif

$(1)_iso.dst: $(2)
	mkdir -v -p $$@
	for f in $$^; do \
		mkdir -v -p "$$@/`dirname $$$$f`"; \
		cp -v -R $$$$f "$$@/$$$$f"; \
	done

$(1).iso: $(1)_iso.dst
	mkisofs -J -o $$@ $$<

CLEAN_TARGETS += $(1)_iso.dst $(1).iso

endef

# ---

define TGTZIP

# Easier to understand error messages.
ifeq ("$$(shell which zip 2>/dev/null)","")
$$(error could not find zip!)
endif

$(1)_zip.dst: $(2)
	mkdir -v -p $$@
	for f in $$^; do \
		mkdir -v -p "$$@/`dirname $$$$f`"; \
		cp -v -R $$$$f "$$@/$$$$f"; \
	done

$(1).zip: $(1)_zip.dst
	zip -r $$@ $$<

CLEAN_TARGETS += $(1)_zip.dst $(1).zip

endef

# ---

define TGTTGZ

# Easier to understand error messages.
ifeq ("$$(shell which tar 2>/dev/null)","")
$$(error could not find tar!)
endif

$(1)_tgz.dst: $(2)
	mkdir -v -p $$@
	for f in $$^; do \
		mkdir -v -p "$$@/`dirname $$$$f`"; \
		cp -v -R $$$$f "$$@/$$$$f"; \
	done

$(1).tgz: $(1)_tgz.dst
	tar -cvzf $$@ $$<

CLEAN_TARGETS += $(1)_tgz.dst $(1).tgz

endef

