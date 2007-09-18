#
# Device Tree Compiler
#

#
# Version information will be constructed in this order:
# EXTRAVERSION might be "-rc", for example.
# LOCAL_VERSION is likely from command line.
# CONFIG_LOCALVERSION from some future config system.
#
VERSION = 1
PATCHLEVEL = 0
SUBLEVEL = 0
EXTRAVERSION =
LOCAL_VERSION =
CONFIG_LOCALVERSION =

DTC_VERSION = $(VERSION).$(PATCHLEVEL).$(SUBLEVEL)$(EXTRAVERSION)
VERSION_FILE = version_gen.h

CONFIG_SHELL := $(shell if [ -x "$$BASH" ]; then echo $$BASH; \
	  else if [ -x /bin/bash ]; then echo /bin/bash; \
	  else echo sh; fi ; fi)

nullstring :=
space	:= $(nullstring) # end of line

localver_config = $(subst $(space),, $(string) \
			      $(patsubst "%",%,$(CONFIG_LOCALVERSION)))

localver_cmd = $(subst $(space),, $(string) \
			      $(patsubst "%",%,$(LOCALVERSION)))

localver_scm = $(shell $(CONFIG_SHELL) ./scripts/setlocalversion)
localver_full  = $(localver_config)$(localver_cmd)$(localver_scm)

dtc_version = $(DTC_VERSION)$(localver_full)

#
# Contents of the generated version file.
#
define filechk_version
	(echo "#define DTC_VERSION \"DTC $(dtc_version)\""; )
endef


CPPFLAGS = -I libfdt
CFLAGS = -Wall -g -Os
LDFLAGS = -Llibfdt

BISON = bison

INSTALL = /usr/bin/install
DESTDIR =
PREFIX = $(HOME)
BINDIR = $(PREFIX)/bin
LIBDIR = $(PREFIX)/lib
INCLUDEDIR = $(PREFIX)/include

#
# Overall rules
#
ifdef V
VECHO = :
else
VECHO = echo "	"
ARFLAGS = rc
.SILENT:
endif

NODEPTARGETS = clean
ifeq ($(MAKECMDGOALS),)
DEPTARGETS = all
else
DEPTARGETS = $(filter-out $(NODEPTARGETS),$(MAKECMDGOALS))
endif

all: dtc ftdump libfdt tests

#
# Rules for dtc proper
#
DTC_PROGS = dtc ftdump
DTC_OBJS = dtc.o flattree.o fstree.o data.o livetree.o \
		srcpos.o treesource.o \
		dtc-parser.tab.o lex.yy.o
DTC_DEPFILES = $(DTC_OBJS:%.o=%.d)

BIN += dtc ftdump

dtc-parser.tab.c dtc-parser.tab.h dtc-parser.output: dtc-parser.y
	@$(VECHO) BISON $@
	@$(VECHO) ---- Expect 2 s/r and 2 r/r. ----
	$(BISON) -d $<

$(VERSION_FILE): Makefile FORCE
	$(call filechk,version)

lex.yy.c: dtc-lexer.l
	@$(VECHO) LEX $@
	$(LEX) $<

dtc: $(DTC_OBJS)

ftdump:	ftdump.o

ifneq ($(DEPTARGETS),)
-include $(DTC_DEPFILES)
endif

#
# Rules for libfdt
#
LIBFDT_PREFIX = libfdt/
include libfdt/Makefile.libfdt

.PHONY: libfdt
libfdt: $(LIBFDT_LIB)

libfdt_clean:
	@$(VECHO) CLEAN "(libfdt)"
	rm -f $(LIBFDT_CLEANFILES)

ifneq ($(DEPTARGETS),)
-include $(LIBFDT_DEPFILES)
endif

#
# Testsuite rules
#
TESTS_PREFIX=tests/
include tests/Makefile.tests

STD_CLEANFILES = *~ *.o *.d *.a *.i *.s core a.out
GEN_CLEANFILES = $(VERSION_FILE)

clean: libfdt_clean tests_clean
	@$(VECHO) CLEAN
	rm -f $(STD_CLEANFILES)
	rm -f $(GEN_CLEANFILES)
	rm -f *.tab.[ch] lex.yy.c *.output vgcore.*
	rm -f $(BIN)

install: all
	@$(VECHO) INSTALL
	$(INSTALL) -d $(DESTDIR)$(BINDIR)
	$(INSTALL) -m 755 dtc $(DESTDIR)$(BINDIR)
	$(INSTALL) -d $(DESTDIR)$(LIBDIR)
	$(INSTALL) -m 644 $(LIBFDT_LIB) $(DESTDIR)$(LIBDIR)
	$(INSTALL) -d $(DESTDIR)$(INCLUDEDIR)
	$(INSTALL) -m 644 $(LIBFDT_INCLUDES) $(DESTDIR)$(INCLUDEDIR)

define filechk
	set -e;					\
	echo '	CHK $@';			\
	mkdir -p $(dir $@);			\
	$(filechk_$(1)) < $< > $@.tmp;		\
	if [ -r $@ ] && cmp -s $@ $@.tmp; then	\
		rm -f $@.tmp;			\
	else					\
		echo '	UPD $@';		\
		mv -f $@.tmp $@;		\
	fi;
endef

#
# Generic compile rules
#
%: %.o
	@$(VECHO) LD $@
	$(LINK.c) -o $@ $^

%.o: %.c
	@$(VECHO) CC $@
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ -c $<

%.o: %.S
	@$(VECHO) AS $@
	$(CC) $(CPPFLAGS) $(AFLAGS) -D__ASSEMBLY__ -o $@ -c $<

%.d: %.c
	$(CC) $(CPPFLAGS) -MM -MG -MT "$*.o $@" $< > $@

%.i:	%.c
	@$(VECHO) CPP $@
	$(CC) $(CPPFLAGS) -E $< > $@

%.s:	%.c
	@$(VECHO) CC -S $@
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ -S $<

%.a:
	@$(VECHO) AR $@
	$(AR) $(ARFLAGS) $@ $^

FORCE:
