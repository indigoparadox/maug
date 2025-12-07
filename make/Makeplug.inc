
# vim: ft=make noexpandtab

# ---

define TGT_WATCOM_WIN32_PLUG

OBJDIR_WATCOM_WIN32_PLUG=obj/watcom-win32-plug

$(1).dll: $$(addprefix $$(OBJDIR_WATCOM_WIN32_PLUG)/,$$(subst .c,.o,$(2)))
	$$(LD_WATCOM386) system nt_dll name $$@ fil {$$^}

$$(OBJDIR_WATCOM_WIN32_PLUG)/%.o: %.c
	$$(MD) "$$(dir $$@)"
	INCLUDE=$(WATCOM)/h $$(CC_WATCOM386) -Isrc -Imaug/src $$(CFLAGS_WATCOM386) \
		-i=$(MAUG_ROOT)/api/file/win -i=$(MAUG_ROOT)/api/log/unix \
		-I$(MAUG_ROOT)/api/mem/win \
		-I$$(WATCOM)/h/nt -bd $$< -fo=$$@ \
		-DRETROFLAT_OS_WIN $$(PLUGIN_DEFINES) -mf -5r -fp5

CLEAN_TARGETS += $(1).dll

endef

# ---

define TGT_GCC_UNIX_PLUG

OBJDIR_GCC_UNIX_PLUG := obj/gcc-$$(MAUG_UNIX)-plug

$(1).$(MAUG_UNIX).so: $$(addprefix $$(OBJDIR_GCC_UNIX_PLUG)/,$$(subst .c,.o,$(2)))
	gcc -fpic -shared -o $$@ $$^

$$(OBJDIR_GCC_UNIX_PLUG)/%.o: %.c
	$$(MD) "$$(dir $$@)"
	$$(CC_GCC) -Isrc -Imaug/src $$(CFLAGS_GCC) \
		-I$(MAUG_ROOT)/api/file/unix -I$(MAUG_ROOT)/api/log/unix \
		-I$(MAUG_ROOT)/api/mem/unix \
		-DRETROFLAT_OS_UNIX $$(PLUGIN_DEFINES) -o $$@ -fpic -c $$^

CLEAN_TARGETS += $(1).so

endef

