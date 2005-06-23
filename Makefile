TARGETS = dtc
CFLAGS = -Wall -g

BISON = bison

OBJS = dtc.o livetree.o flattree.o data.o treesource.o fstree.o \
	dtc-parser.tab.o lex.yy.o

all: $(TARGETS)

dtc:	$(OBJS)
	$(LINK.c) -o $@ $^

$(OBJS): dtc.h

dtc-parser.tab.c dtc-parser.tab.h dtc-parser.output: dtc-parser.y
	$(BISON) -d -v $<

lex.yy.c: dtc-lexer.l
	$(LEX) $<

lex.yy.o: lex.yy.c dtc-parser.tab.h

livetree.o:	flat_dt.h

check: all
	cd tests && $(MAKE) check

clean:
	rm -f *~ *.o a.out core $(TARGETS)
	rm -f *.tab.[ch] lex.yy.c
	rm -f *.i *.output vgcore.*
	cd tests && $(MAKE) clean

