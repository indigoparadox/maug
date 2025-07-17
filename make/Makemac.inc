
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
	-I$(MAUG_ROOT)/api/file/mac68k -I$(MAUG_ROOT)/api/log/mac68k \
	-I$(MAUG_ROOT)/api/font/mac68k
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

ifeq ("$(4)","")
MAUG_MAC_CREATOR := MAUG
else
MAUG_MAC_CREATOR := $(4)
endif

ifeq ("$(3)","")
# Use generic maug icon if not specified.
$$(OBJDIR_GCC_MAC_M68K)/icon.r: $(MAUG_ROOT)/maug32.bmp
	$(MAUG_ROOT)/tools/bmp.py to_icn -x $$< $$@
else
$$(OBJDIR_GCC_MAC_M68K)/icon.r: $(3)
	$(MAUG_ROOT)/tools/bmp.py to_icn -x $$< $$@
endif

$$(OBJDIR_GCC_MAC_M68K)/macrsrc.r: $(MAUG_ROOT)/scraps/macrsrc.r | \
$$(OBJDIR_GCC_MAC_M68K)/icon.r
	cp -v $$< $$@
	sed -i "s|zzz|$$(MAUG_MAC_CREATOR)|g" $$@
	cat $$(OBJDIR_GCC_MAC_M68K)/icon.r >> $$@

$$(OBJDIR_GCC_MAC_M68K)/$(1).APPL \
$$(OBJDIR_GCC_MAC_M68K)/$(1).bin \
$(1).m68k.dsk: \
$$(OBJDIR_GCC_MAC_M68K)/$(1).code.bin | $$(OBJDIR_GCC_MAC_M68K)/macrsrc.r
	Rez -I$(RINCLUDES) --copy "$$<" "$$(OBJDIR_GCC_MAC_M68K)/macrsrc.r" \
		-t "APPL" -c "$$(MAUG_MAC_CREATOR)" -o $$(OBJDIR_GCC_MAC_M68K)/$(1).bin \
		--cc $$(OBJDIR_GCC_MAC_M68K)/$(1).APPL --cc $(1).m68k.dsk
	if [ -n "$(2)" ]; then \
		hmount $(1).m68k.dsk && \
		for f in $(2); do \
			md="`dirname "$$$${f}" | sed 's|/|:|g'`"; \
			mf="`echo "$$$${f}" | sed 's|/|:|g'`"; \
			if file $$$${f} | grep -q ASCII || file $$$${f} | grep -q JSON; then \
				ma="-t"; \
			else \
				ma=""; \
			fi; \
			hmkdir :$$$${md} 2>/dev/null ; \
			echo hcopy $$$$ma $$$$f :$$$$mf ; \
			hcopy $$$$ma $$$$f :$$$$mf ; \
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

