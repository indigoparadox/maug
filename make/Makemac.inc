
# vim: ft=make noexpandtab

CC_MAC68K := /usr/m68k-apple-macos/bin/m68k-apple-macos-gcc

# Target: Mac System 7 API (68k)
define TGTMAC68K

OBJDIR_GCC_MAC_M68K=obj/gcc-mac-mac68k

CFLAGS_GCC_MAC_M68K := \
	-DRETROFLAT_API_MAC \
	-I$(MAUG_ROOT)/api/retro2d/mac68k -I$(MAUG_ROOT)/api/input/mac68k
# TODO: Make math link optional.
LDFLAGS_GCC_MAC_M68K := -lm

ifeq ("$$(RETROFLAT_SOUND)","1")
	CFLAGS_GCC_MAC_M68K += -I$(MAUG_ROOT)/api/sound/null
endif

$(1).m68k.dsk: \
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


