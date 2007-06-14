TARGETS = dtc ftdump
CFLAGS = -Wall -g

BISON = bison

DTC_OBJS = dtc.o flattree.o fstree.o data.o livetree.o \
		srcpos.o treesource.o \
		dtc-parser.tab.o lex.yy.o

DEPFILES = $(DTC_OBJS:.o=.d)

.PHONY: libfdt tests

all: $(TARGETS) tests libfdt

dtc: $(DTC_OBJS)
	$(LINK.c) -o $@ $^

ftdump:	ftdump.o
	$(LINK.c) -o $@ $^

libfdt:
	cd libfdt && $(MAKE) all

dtc-parser.tab.c dtc-parser.tab.h dtc-parser.output: dtc-parser.y
	$(BISON) -d $<

lex.yy.c: dtc-lexer.l
	$(LEX) $<

lex.yy.o: lex.yy.c dtc-parser.tab.h

tests:	tests/all

tests/%: libfdt
	$(MAKE) -C tests $*

check:	all
	cd tests; ./run_tests.sh

checkv:	all
	cd tests; ./run_tests.sh -v

func:	all
	cd tests; ./run_tests.sh -t func

funcv:	all
	cd tests; ./run_tests.sh -t func -v

stress:	all
	cd tests; ./run_tests.sh -t stress

stressv: all
	cd tests; ./run_tests.sh -t stress -v

clean:
	rm -f *~ *.o a.out core $(TARGETS)
	rm -f *.tab.[ch] lex.yy.c
	rm -f *.i *.output vgcore.*
	rm -f *.d
	$(MAKE) -C libfdt clean
	$(MAKE) -C tests clean

%.d: %.c
	$(CC) $(CPPFLAGS) -MM -MG -MT "$*.o $@" $< > $@

-include $(DEPFILES)
