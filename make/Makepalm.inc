
# vim: ft=make noexpandtab

CC_PALM := m68k-palmos-gcc

LD_PALM := m68k-palmos-gcc

OBJDIR_PALM := obj/palmos

INCLUDES_PALM := \
   -I $(PALMSDK)/sdk-3.5/include \
   -I $(PALMSDK)/sdk-3.5/include/Libraries/ \
   -I $(PALMSDK)/sdk-3.5/include/Dynamic/ \
   -I $(PALMSDK)/sdk-3.5/include/Core/ \
   -I $(PALMSDK)/sdk-3.5/include/Core/UI/ \
   -I $(PALMSDK)/sdk-3.5/include/Core/System/ \
   -I $(PALMSDK)/sdk-3.5/include/Core/Hardware/ \
   -I $(PALMSDK)/sdk-3.5/include/Core/International/ 

CFLAGS_PALM += -I "$(MAUG_ROOT)/src" -I "$(MAUG_ROOT)/api/palm" $(INCLUDES_PALM) -DRETROFLAT_OS_PALM -DRETROFLAT_API_PALM -DRETROFLAT_NO_CLI -DNO_RETROSND -DMAUG_NO_ASSERT -DRETROCON_DISABLE

LDFLAGS_PALM +=

# ---

define TGT_ICO_PALM

# General utility macro to be called below to pick up the application icon
# (<appname>.bmp) *or* the default maug icon if no application icon present.

$(2)/win_ico.ico:
	$$(MD) $$(dir $$@)
	if [ -f "$$<.bmp" ]; then \
		convert "$$<.bmp" -transparent black "$$@"; \
	else \
		convert "$$(MAUG_ROOT)/maug16.bmp" -transparent black "$$@"; \
	fi

$(2)/$(1).rc: $(2)/win_ico.ico
	echo "$$(WIN_ICO_RES_ID)   ICON   \"$(2)/win_ico.ico\"" > $$@

endef

# ---

define TGTPALM

#$$(addprefix $$(OBJDIR_CECL_WINCE_SH3)/,$$(subst .c,.o,$$(C_FILES))) | \
#$$(OBJDIR_CECL_WINCE_SH3)/$(1).res
#	$$(LD_CECL) \
#		$$(LDFLAGS_CECL_WINCE_SH3) \
#		$$(LDFLAGS_CECL) $$(LDFLAGS_CECL_WINCE) \
#		$$(OBJDIR_CECL_WINCE_SH3)/$(1).res \
#		$$^ /OUT:$$@

$$(OBJDIR_PALM)/$(1).elf: \
$$(addprefix $$(OBJDIR_PALM)/,$$(subst .c,.o,$$(C_FILES))) | $$(MAUG_DEPS)
	$$(LD_PALM) $$(LDFLAGS_PALM) -o $$@ $$^

$$(OBJDIR_PALM)/%.o: %.c | $$(MAUG_DEPS)
	$$(MD) "$$(dir $$@)"
	$$(CC_PALM) $$(CFLAGS_GCC) $$(CFLAGS_PALM) -c -o $$@ $$<

CLEAN_TARGETS += $(1).prc

endef

