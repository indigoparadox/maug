
# vim: ft=make noexpandtab

ifndef RETRO68
RETRO68 := $(subst bin/m68k-apple-macos-gcc,,$(shell which m68k-apple-macos-gcc))
RINCLUDES := $(RETRO68)RIncludes
endif

CC_MAC68K := $(RETRO68)bin/m68k-apple-macos-gcc

CC_MACPPC := $(RETRO68)bin/powerpc-apple-macos-gcc

# ---

# Target: Mac System 6 API (68k)
define TGTMAC68K

OBJDIR_GCC_MAC68K_TB6=obj/gcc-mac68k-tb6

CFLAGS_GCC_MAC68K_TB6 := \
	-DRETROFLAT_API_MAC6 -DRETROFLAT_MAC_NO_DBLBUF -DRETROANI_DISABLE \
	-I$(MAUG_ROOT)/api/retro2d/mac68k -I$(MAUG_ROOT)/api/input/mac68k \
	-I$(MAUG_ROOT)/api/log/mac68k \
	-I$(MAUG_ROOT)/api/font/mac68k -I$(MAUG_ROOT)/api/mem/unix

# TODO: Make math link optional.
LDFLAGS_GCC_MAC68K_TB6 := -lm

ifneq ("$$(BUILD)","RELEASE")
ifneq ("$$(MAC_FORCE_CONSOLE_LOG)","1")
	CFLAGS_GCC_MAC68K_TB6 += -DLOG_TO_FILE -DLOG_FILE_NAME=\"out.log\"
endif
endif

ifeq ("$$(FORCE_MVFS)","1")
CFLAGS_GCC_MAC68K_TB6 += -I"$(MAUG_ROOT)/api/file/mvfs"
else
CFLAGS_GCC_MAC68K_TB6 += -I"$(MAUG_ROOT)/api/file/mac68k"
endif

ifeq ("$$(RETROFLAT_SOUND)","1")
	CFLAGS_GCC_MAC68K_TB6 += -I$(MAUG_ROOT)/api/sound/null
endif

ifeq ("$(4)","")
MAUG_MAC_CREATOR := MAUG
else
MAUG_MAC_CREATOR := $(4)
endif

ifeq ("$(3)","")
# Use generic maug icon if not specified.
$$(OBJDIR_GCC_MAC68K_TB6)/icon.r: $(MAUG_ROOT)/maug32.bmp
	$(MAUG_ROOT)/tools/bmp.py to_icn -x $$< $$@
else
$$(OBJDIR_GCC_MAC68K_TB6)/icon.r: $(3)
	$(MAUG_ROOT)/tools/bmp.py to_icn -x $$< $$@
endif

$$(OBJDIR_GCC_MAC68K_TB6)/macrsrc.r: $(MAUG_ROOT)/scraps/macrsrc.r | \
$$(OBJDIR_GCC_MAC68K_TB6)/icon.r
	cp -v $$< $$@
	sed -i "s|zzz|$$(MAUG_MAC_CREATOR)|g" $$@
	cat $$(OBJDIR_GCC_MAC68K_TB6)/icon.r >> $$@

$$(OBJDIR_GCC_MAC68K_TB6)/$(1).APPL \
$$(OBJDIR_GCC_MAC68K_TB6)/$(1).bin \
$(1).m68k.dsk: \
$$(OBJDIR_GCC_MAC68K_TB6)/$(1).code.bin | $$(OBJDIR_GCC_MAC68K_TB6)/macrsrc.r
	Rez -I$(RINCLUDES) --copy "$$<" "$$(OBJDIR_GCC_MAC68K_TB6)/macrsrc.r" \
		-t "APPL" -c "$$(MAUG_MAC_CREATOR)" -o $$(OBJDIR_GCC_MAC68K_TB6)/$(1).bin \
		--cc $$(OBJDIR_GCC_MAC68K_TB6)/$(1).APPL --cc $(1).m68k.dsk
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

$$(OBJDIR_GCC_MAC68K_TB6)/$(1).code.bin: \
$$(addprefix $$(OBJDIR_GCC_MAC68K_TB6)/,$$(subst .c,.o,$$(C_FILES)))
	$$(CC_MAC68K) -o $$@ $$^ \
		$$(subst -pg,,$$(subst -g,,$$(LDLAGS_GCC))) \
		$$(LDFLAGS_GCC_MAC68K_TB6)

$$(OBJDIR_GCC_MAC68K_TB6)/%.o: %.c | $$(MAUG_DEPS)
	$$(MD) $$(dir $$@)
	$$(CC_MAC68K) -c -o $$@ $$< \
		$$(subst -pg,,$$(subst -g,,$$(CFLAGS_GCC))) \
		$$(CFLAGS_GCC_MAC68K_TB6) $$(CFLAGS_OPT_GCC)

CLEAN_TARGETS += $(1).m68k.dsk

endef

# ---

# Target: Mac System 7 API (68k)
define TGTMAC68K7

OBJDIR_GCC_MAC68K_TB7=obj/gcc-mac68k-tb7

CFLAGS_GCC_MAC68K_TB7 := \
	-DRETROFLAT_API_MAC7 -DRETROANI_DISABLE \
	-I$(MAUG_ROOT)/api/retro2d/mac68k -I$(MAUG_ROOT)/api/input/mac68k \
	-I$(MAUG_ROOT)/api/log/mac68k \
	-I$(MAUG_ROOT)/api/font/mac68k -I$(MAUG_ROOT)/api/mem/unix

# TODO: Make math link optional.
LDFLAGS_GCC_MAC68K_TB7 := -lm

ifneq ("$$(BUILD)","RELEASE")
ifneq ("$$(MAC_FORCE_CONSOLE_LOG)","1")
	CFLAGS_GCC_MAC68K_TB7 += -DLOG_TO_FILE -DLOG_FILE_NAME=\"out.log\"
endif
endif

ifeq ("$$(FORCE_MVFS)","1")
CFLAGS_GCC_MAC68K_TB7 += -I"$(MAUG_ROOT)/api/file/mvfs"
else
CFLAGS_GCC_MAC68K_TB7 += -I"$(MAUG_ROOT)/api/file/mac68k"
endif

ifeq ("$$(RETROFLAT_SOUND)","1")
	CFLAGS_GCC_MAC68K_TB7 += -I$(MAUG_ROOT)/api/sound/null
endif

ifeq ("$(4)","")
MAUG_MAC_CREATOR := MAUG
else
MAUG_MAC_CREATOR := $(4)
endif

ifeq ("$(3)","")
# Use generic maug icon if not specified.
$$(OBJDIR_GCC_MAC68K_TB7)/icon.r: $(MAUG_ROOT)/maug32.bmp
	$(MAUG_ROOT)/tools/bmp.py to_icn -x $$< $$@
else
$$(OBJDIR_GCC_MAC68K_TB7)/icon.r: $(3)
	$(MAUG_ROOT)/tools/bmp.py to_icn -x $$< $$@
endif

$$(OBJDIR_GCC_MAC68K_TB7)/macrsrc.r: $(MAUG_ROOT)/scraps/macrsrc.r | \
$$(OBJDIR_GCC_MAC68K_TB7)/icon.r
	cp -v $$< $$@
	sed -i "s|zzz|$$(MAUG_MAC_CREATOR)|g" $$@
	cat $$(OBJDIR_GCC_MAC68K_TB7)/icon.r >> $$@

$$(OBJDIR_GCC_MAC68K_TB7)/$(1).APPL \
$$(OBJDIR_GCC_MAC68K_TB7)/$(1).bin \
$(1).mac7.dsk: \
$$(OBJDIR_GCC_MAC68K_TB7)/$(1).code.bin | $$(OBJDIR_GCC_MAC68K_TB7)/macrsrc.r
	Rez -I$(RINCLUDES) --copy "$$<" "$$(OBJDIR_GCC_MAC68K_TB7)/macrsrc.r" \
		-t "APPL" -c "$$(MAUG_MAC_CREATOR)" -o $$(OBJDIR_GCC_MAC68K_TB7)/$(1).bin \
		--cc $$(OBJDIR_GCC_MAC68K_TB7)/$(1).APPL --cc $(1).mac7.dsk
	if [ -n "$(2)" ]; then \
		hmount $(1).mac7.dsk && \
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
		done; humount $(1).mac7.dsk; fi

$$(OBJDIR_GCC_MAC68K_TB7)/$(1).code.bin: \
$$(addprefix $$(OBJDIR_GCC_MAC68K_TB7)/,$$(subst .c,.o,$$(C_FILES)))
	$$(CC_MAC68K) -o $$@ $$^ \
		$$(subst -pg,,$$(subst -g,,$$(LDLAGS_GCC))) \
		$$(LDFLAGS_GCC_MAC68K_TB7)

$$(OBJDIR_GCC_MAC68K_TB7)/%.o: %.c | $$(MAUG_DEPS)
	$$(MD) $$(dir $$@)
	$$(CC_MAC68K) -c -o $$@ $$< \
		$$(subst -pg,,$$(subst -g,,$$(CFLAGS_GCC))) \
		$$(CFLAGS_GCC_MAC68K_TB7) $$(CFLAGS_OPT_GCC)

CLEAN_TARGETS += $(1).mac7.dsk

endef

# ---

# Target: Mac System 7 API (PowerPC)
define TGTMACPPC

OBJDIR_GCC_MACPPC_CARBON=obj/gcc-macppc-carbon

CFLAGS_GCC_MACPPC_CARBON := \
	-DRETROFLAT_API_MAC \
	-I$(MAUG_ROOT)/api/retro2d/mac68k -I$(MAUG_ROOT)/api/input/mac68k \
	-I$(MAUG_ROOT)/api/log/mac68k \
	-I$(MAUG_ROOT)/api/font/mac68k -I$(MAUG_ROOT)/api/mem/unix

# TODO: Make math link optional.
LDFLAGS_GCC_MACPPC_CARBON := -lm

ifneq ("$$(BUILD)","RELEASE")
ifneq ("$$(MAC_FORCE_CONSOLE_LOG)","1")
	CFLAGS_GCC_MACPPC_CARBON += -DLOG_TO_FILE -DLOG_FILE_NAME=\"out.log\"
endif
endif

ifeq ("$$(FORCE_MVFS)","1")
CFLAGS_GCC_MACPPC_CARBON += -I"$(MAUG_ROOT)/api/file/mvfs"
else
CFLAGS_GCC_MACPPC_CARBON += -I"$(MAUG_ROOT)/api/file/mac68k"
endif

ifeq ("$$(RETROFLAT_SOUND)","1")
	CFLAGS_GCC_MACPPC_CARBON += -I$(MAUG_ROOT)/api/sound/null
endif

ifeq ("$(4)","")
MAUG_MAC_CREATOR := MAUG
else
MAUG_MAC_CREATOR := $(4)
endif

ifeq ("$(3)","")
# Use generic maug icon if not specified.
$$(OBJDIR_GCC_MACPPC_CARBON)/icon.r: $(MAUG_ROOT)/maug32.bmp
	$(MAUG_ROOT)/tools/bmp.py to_icn -x $$< $$@
else
$$(OBJDIR_GCC_MACPPC_CARBON)/icon.r: $(3)
	$(MAUG_ROOT)/tools/bmp.py to_icn -x $$< $$@
endif

$$(OBJDIR_GCC_MACPPC_CARBON)/macrsrc.r: $(MAUG_ROOT)/scraps/macrsrc.r | \
$$(OBJDIR_GCC_MACPPC_CARBON)/icon.r
	cp -v $$< $$@
	sed -i "s|zzz|$$(MAUG_MAC_CREATOR)|g" $$@
	cat $$(OBJDIR_GCC_MACPPC_CARBON)/icon.r >> $$@

$$(OBJDIR_GCC_MACPPC_CARBON)/$(1).APPL \
$$(OBJDIR_GCC_MACPPC_CARBON)/$(1).bin \
$(1).ppc.dsk: \
$$(OBJDIR_GCC_MACPPC_CARBON)/$(1).xcoff | $$(OBJDIR_GCC_MACPPC_CARBON)/macrsrc.r
	Rez -I$(RINCLUDES) "$$(OBJDIR_GCC_MACPPC_CARBON)/macrsrc.r" \
		-t "APPL" -c "$$(MAUG_MAC_CREATOR)" -o $$(OBJDIR_GCC_MACPPC_CARBON)/$(1).bin \
		--cc $$(OBJDIR_GCC_MACPPC_CARBON)/$(1).APPL --cc $(1).ppc.dsk \
		--data $$<
	if [ -n "$(2)" ]; then \
		hmount $(1).ppc.dsk && \
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
		done; humount $(1).ppc.dsk; fi

$$(OBJDIR_GCC_MACPPC_CARBON)/$(1).xcoff: \
$$(addprefix $$(OBJDIR_GCC_MACPPC_CARBON)/,$$(subst .c,.o,$$(C_FILES)))
	$$(CC_MACPPC) -o $$@ $$^ \
		$$(subst -pg,,$$(subst -g,,$$(LDLAGS_GCC))) \
		$$(LDFLAGS_GCC_MACPPC_CARBON)

$$(OBJDIR_GCC_MACPPC_CARBON)/%.o: %.c | $$(MAUG_DEPS)
	$$(MD) $$(dir $$@)
	$$(CC_MACPPC) -c -o $$@ $$< \
		$$(subst -pg,,$$(subst -g,,$$(CFLAGS_GCC))) \
		$$(CFLAGS_GCC_MACPPC_CARBON) $$(CFLAGS_OPT_GCC)

CLEAN_TARGETS += $(1).ppc.dsk

endef

