TARGETS = dtc
CFLAGS = -Wall -g

OBJS = dtc.o livetree.o flattree.o data.o treesource.o fstree.o \
	y.tab.o lex.yy.o

all: $(TARGETS)

dtc:	$(OBJS)
	$(LINK.c) -o $@ $^

$(OBJS): dtc.h

y.tab.c y.tab.h: dtc-parser.y
	$(YACC) -d $<

lex.yy.c: dtc-lexer.l
	$(LEX) $<

lex.yy.o: lex.yy.c y.tab.h

dtc-parser.c:	dtc-lexer.c

check: all
	cd tests && $(MAKE) check

clean:
	rm -f *~ *.o a.out core $(TARGETS)
	rm -f *.tab.[ch] lex.yy.c
	rm -f *.i *.output vgcore.*
	cd tests && $(MAKE) clean

