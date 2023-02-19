
# vim: ft=make noexpandtab

ifeq ("$(CFLAGS_WATCOM_OS2)","")
	CFLAGS_WATCOM_OS2 := -DRETROFLAT_OS_OS2
endif
ifeq ("$(LDFLAGS_WATCOM_OS2)","")
	LDFLAGS_WATCOM_OS2 :=
endif

ifeq ("$(CFLAGS_WATCOM_OS2_PM)","")
	CFLAGS_WATCOM_OS2_PM := -DRETROFLAT_API_OS2PM
endif

# ---

# ---

define TGTOS2PM

$(1)2.exe: $$(addprefix obj/os2/,$$(subst .c,.o,$$(C_FILES)))
	wlink name $$@ system os2_pm \
		$$(LDFLAGS_WATCOM_OS2) $$(LDFLAGS_WATCOM_OS2_PM) fil {$$^}

obj/os2/%.o: %.c
	mkdir -p $(dir $@)
	wcc386 $$(CFLAGS_WATCOM) $$(CFLAGS_WATCOM_OS2) $$(CFLAGS_WATCOM_OS2_PM) \
		-bt=os2 -i=$(WATCOM)/h/os2 -fo=$@ $(<:%.c=%)

endef

