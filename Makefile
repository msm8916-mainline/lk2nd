CPPFLAGS = -I libfdt
CFLAGS = -Wall -g
LDFLAGS = -Llibfdt

BISON = bison

INSTALL = /usr/bin/install
DESTDIR =
BINDIR = /usr/bin

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

STD_CLEANFILES = *~ *.o *.d *.a *.i *.s core a.out

clean: libfdt_clean tests_clean
	@$(VECHO) CLEAN
	rm -f $(STD_CLEANFILES)
	rm -f *.tab.[ch] lex.yy.c *.output vgcore.*
	rm -f $(BIN)

#
# General rules
#

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

$(BIN): %:
	@$(VECHO) LD $@
	$(LINK.c) -o $@ $^


#
# Rules for dtc proper
#
DTC_PROGS = dtc ftdump
DTC_OBJS = dtc.o flattree.o fstree.o data.o livetree.o \
		srcpos.o treesource.o \
		dtc-parser.tab.o lex.yy.o
DTC_DEPFILES = $(DTC_OBJS:%.o=%.d)

dtc-parser.tab.c dtc-parser.tab.h dtc-parser.output: dtc-parser.y
	@$(VECHO) BISON $@
	$(BISON) -d $<

lex.yy.c: dtc-lexer.l
	@$(VECHO) LEX $@
	$(LEX) $<

BIN += dtc ftdump

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

install: dtc ftdump
	$(INSTALL) -d $(DESTDIR)$(BINDIR)
	$(INSTALL) -m 755 dtc $(DESTDIR)$(BINDIR)
	$(INSTALL) -m 755 ftdump $(DESTDIR)$(BINDIR)
