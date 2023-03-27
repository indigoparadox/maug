
# vim: ft=make noexpandtab

CC_GCC_MAC6 := m68k-apple-macos-gcc
CXX_GCC_MAC6 := m68k-apple-macos-g++

CFLAGS_GCC_MAC := -DRETROFLAT_OS_MAC

CFLAGS_GCC_MAC_MAC6 := -DRETROFLAT_API_MAC6 -DRETROCON_DISABLE

OBJDIR_GCC_MAC_MAC6=obj/gcc-mac-mac6

# ---

define TGT_GCC_MAC_MAC6

MAC6_CREATOR := $$(shell echo "$(1)" | head -c 4)

$(1).bin $(1).APPL $(1).dsk: $(OBJDIR_GCC_MAC_MAC6)/$(1).code.bin
	Rez -I$$(RETRO68)/RIncludes \
      --copy $$^ \
      "$$(RETRO68)/RIncludes/Retro68APPL.r" \
      -t "APPL" -c "$$(MAC6_CREATOR)" \
      -o $(1).bin \
      --cc $(1).APPL \
      --cc $(1).dsk

$$(OBJDIR_GCC_MAC_MAC6)/$(1).code.bin: \
$$(addprefix $$(OBJDIR_GCC_MAC_MAC6)/,$$(subst .c,.o,$$(C_FILES)))
	$$(CXX_GCC_MAC6) -o $$@ $$^ $$(LDFLAGS_GCC_MAC) $$(LDFLAGS_GCC_MAC_MAC6)

$$(OBJDIR_GCC_MAC_MAC6)/%.o: %.c | $(MAUG_DEPS)
	$$(MD) "$$(dir $$@)"
	$$(CC_GCC_MAC6) \
		$$(CFLAGS_GCC) $$(CFLAGS_GCC_MAC) $$(CFLAGS_GCC_MAC_MAC6) \
		-o $$@ -c $$<

CLEAN_TARGETS += $(1).bin $(1).APPL $(1).dsk

endef

# Legacy
TGTMAC6GCC=$(TGT_GCC_MAC_MAC6)

