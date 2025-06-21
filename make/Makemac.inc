
# vim: ft=make noexpandtab

ifndef RETRO68
RETRO68 := $(subst bin/m68k-apple-macos-gcc,,$(shell which m68k-apple-macos-gcc))
RINCLUDES := $(RETRO68)RIncludes
endif

CC_MAC68K := $(RETRO68)bin/m68k-apple-macos-gcc

# Target: Mac System 7 API (68k)
define TGTMAC68K

OBJDIR_GCC_MAC_M68K=obj/gcc-mac-mac68k

CFLAGS_GCC_MAC_M68K := \
	-DRETROFLAT_API_MAC \
	-I$(MAUG_ROOT)/api/retro2d/mac68k -I$(MAUG_ROOT)/api/input/mac68k \
	-I$(MAUG_ROOT)/api/file/unix -I$(MAUG_ROOT)/api/log/unix
# TODO: Make math link optional.
LDFLAGS_GCC_MAC_M68K := -lm

ifneq ("$(BUILD)","RELEASE")
ifneq ("$(MAC_FORCE_CONSOLE_LOG)","1")
	CFLAGS_GCC_MAC_M68K += -DLOG_TO_FILE -DLOG_FILE_NAME=\"out.log\"
endif
endif

ifeq ("$$(RETROFLAT_SOUND)","1")
	CFLAGS_GCC_MAC_M68K += -I$(MAUG_ROOT)/api/sound/null
endif

$$(OBJDIR_GCC_MAC_M68K)/$(1).APPL \
$$(OBJDIR_GCC_MAC_M68K)/$(1).bin \
$(1).m68k.dsk: \
$$(OBJDIR_GCC_MAC_M68K)/$(1).code.bin
	Rez -I$(RINCLUDES) --copy "$$<" "$(RINCLUDES)/Retro68APPL.r" \
		-t "APPL" -c "????" -o $$(OBJDIR_GCC_MAC_M68K)/$(1).bin \
		--cc $$(OBJDIR_GCC_MAC_M68K)/$(1).APPL --cc $(1).m68k.dsk
	if [ -n "$(2)" ]; then \
		hmount $(1).m68k.dsk && \
		for f in $(2); do \
			md="`dirname "$$$${f}" | sed 's|/|:|g'`"; \
			mf="`echo "$$$${f}" | sed 's|/|:|g'`"; \
			hmkdir :$$$${md} ; \
			hcopy $$$$f :$$$$mf ; \
		done; humount $(1).m68k.dsk; fi

$$(OBJDIR_GCC_MAC_M68K)/$(1).code.bin: \
$$(addprefix $$(OBJDIR_GCC_MAC_M68K)/,$$(subst .c,.o,$$(C_FILES)))
	$$(CC_MAC68K) -o $$@ $$^ \
		$$(subst -pg,,$$(subst -g,,$$(LDLAGS_GCC))) \
		$$(LDFLAGS_GCC_MAC_M68K)

$$(OBJDIR_GCC_MAC_M68K)/%.o: %.c | $$(MAUG_DEPS)
	$$(MD) $$(dir $$@)
	$$(CC_MAC68K) -c -o $$@ $$< \
		$$(subst -pg,,$$(subst -g,,$$(CFLAGS_GCC))) \
		$$(CFLAGS_GCC_MAC_M68K) $$(CFLAGS_OPT_GCC)

CLEAN_TARGETS += $(1).m68k.dsk

endef

