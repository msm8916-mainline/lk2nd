# comment out or override if you want to see the full output of each command
NOECHO ?= @

$(OUTBIN): $(OUTELF)
	@echo generating image: $@
	$(NOECHO)$(SIZE) $<
	$(NOCOPY)$(OBJCOPY) -O binary $< $@

ifeq ($(ENABLE_TRUSTZONE), 1)
$(OUTELF): $(ALLOBJS) $(LINKER_SCRIPT) $(OUTPUT_TZ_BIN)
	@echo linking $@
	$(NOECHO)$(LD) $(LDFLAGS) -T $(LINKER_SCRIPT) $(OUTPUT_TZ_BIN) $(ALLOBJS) $(LIBGCC) -o $@
else
$(OUTELF): $(ALLOBJS) $(LINKER_SCRIPT)
	@echo linking $@
	$(NOECHO)$(LD) $(LDFLAGS) -T $(LINKER_SCRIPT) $(ALLOBJS) $(LIBGCC) -o $@
endif


$(OUTELF).sym: $(OUTELF)
	@echo generating symbols: $@
	$(NOECHO)$(OBJDUMP) -t $< | $(CPPFILT) > $@

$(OUTELF).lst: $(OUTELF)
	@echo generating listing: $@
	$(NOECHO)$(OBJDUMP) -Mreg-names-raw -d $< | $(CPPFILT) > $@

$(OUTELF).debug.lst: $(OUTELF)
	@echo generating listing: $@
	$(NOECHO)$(OBJDUMP) -Mreg-names-raw -S $< | $(CPPFILT) > $@

$(OUTELF).size: $(OUTELF)
	@echo generating size map: $@
	$(NOECHO)$(NM) -S --size-sort $< > $@

ifeq ($(ENABLE_TRUSTZONE), 1)
$(OUTPUT_TZ_BIN): $(INPUT_TZ_BIN)
	@echo generating TZ output from TZ input
	$(NOECHO)$(OBJCOPY) -I binary -B arm -O elf32-littlearm $(INPUT_TZ_BIN) $(OUTPUT_TZ_BIN)
endif

$(OUTELF_STRIP): $(OUTELF)
	@echo generating stripped elf: $@
	$(NOECHO)$(STRIP) -S $< -o $@

$(BUILDDIR)/%.dtb: %.dts
	@$(MKDIR)
	@echo compiling $<
	$(NOECHO)dtc -O dtb -i dts -o $@ $<

$(OUTDTIMG): $(DTBS)
	$(NOECHO)scripts/dtbTool -o $@ $(BUILDDIR)/dts/$(TARGET)

$(OUTBOOTIMG): $(OUTBIN) $(OUTDTIMG)
	$(NOECHO)scripts/mkbootimg \
		--kernel=$(OUTBIN) \
		--ramdisk=/dev/null \
		--dt=$(OUTDTIMG) \
		--base=$(ANDROID_BOOT_BASE) \
		--output=$@ \
		--cmdline="$(ANDROID_BOOT_CMDLINE)"
	$(NOECHO)echo -n SEANDROIDENFORCE >> $@

$(OUTODINTAR): $(OUTBOOTIMG)
	$(NOECHO)tar \
		-cf $@ --sort=name --mtime="@0" \
		--owner=0 --group=0 --numeric-owner \
		--transform="flags=r;s|$(OUTBOOTIMG)|boot.img|" \
		$(OUTBOOTIMG)

include arch/$(ARCH)/compile.mk
