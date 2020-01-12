# comment out or override if you want to see the full output of each command
NOECHO ?= @

$(OUTBIN): $(OUTELF)
	@echo generating image: $@
	$(NOECHO)$(SIZE) $<
	$(NOCOPY)$(OBJCOPY) -O binary $< $@

$(OUTZIMAGEDTB): $(OUTBIN) $(DTBS)
	@echo generating zImage+dtb: $@
	$(NOECHO)gzip -c $(OUTBIN) > $@
	$(NOECHO)cat $(DTBS) >> $@

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
	$(NOECHO)dtc -O dtb -o $@ $<

$(OUTDTIMG): $(DTBS)
	$(NOECHO)scripts/dtbTool -o $@ $(BUILDDIR)/dts

ifeq ($(TARGET_USES_APPENDED_DTBS),1)
$(OUTBOOTIMG): $(OUTBIN) $(OUTZIMAGEDTB)
	$(NOECHO)scripts/mkbootimg \
		--kernel=$(OUTZIMAGEDTB) \
		--ramdisk=/dev/null \
		--base=$(ANDROID_BOOT_BASE) \
		--output=$@ \
		--cmdline="$(ANDROID_BOOT_CMDLINE)"
	$(NOECHO)echo -n SEANDROIDENFORCE >> $@
else
$(OUTBOOTIMG): $(OUTBIN) $(OUTDTIMG)
	$(NOECHO)scripts/mkbootimg \
		--kernel=$(OUTBIN) \
		--ramdisk=/dev/null \
		--dt=$(OUTDTIMG) \
		--base=$(ANDROID_BOOT_BASE) \
		--output=$@ \
		--cmdline="$(ANDROID_BOOT_CMDLINE)"
	$(NOECHO)echo -n SEANDROIDENFORCE >> $@
endif


include arch/$(ARCH)/compile.mk
