PREFIX = /usr/local

LIB_TESTS = root_node find_property subnode_offset path_offset getprop \
	notfound \
	setprop_inplace nop_property nop_node \
	sw_tree1 \
	move_and_save \
	open_pack rw_tree1 setprop del_property del_node
LIBTREE_TESTS = truncated_property
TESTS = $(LIB_TESTS) $(LIBTREE_TESTS)
UTILS = dumptrees

TREES = test_tree1.dtb

CFLAGS = -Wall -g
CPPFLAGS = -I..
LDFLAGS = -L..

LIBFDT = ../libfdt.a

ifdef V
VECHO = :
else
VECHO = echo "	"
.SILENT:
endif

DEPFILES = $(TESTS:%=%.d) testutils.d

all:	$(TESTS) $(TREES)

%.o: %.c
	@$(VECHO) CC $@
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ -c $<

%.o: %.S
	@$(VECHO) AS $@
	$(CC) -D__ASSEMBLY__ $(CPPFLAGS) -o $@ -c $<

%: %.o
	@$(VECHO) LD $@
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(LIB_TESTS): %: testutils.o $(LIBFDT)

$(LIBTREE_TESTS): %: testutils.o trees.o $(LIBFDT)

dumptrees: %: trees.o

$(TREES): dumptrees
	@$(VECHO) DUMPTREES
	./dumptrees >/dev/null

clean:
	@$(VECHO) CLEAN "(tests)"
	rm -f *~ *.o *.so *.a *.d *.s core a.out
	rm -f $(TESTS) $(UTILS) *.dtb

%.d: %.c
	@$(CC) $(CPPFLAGS) -MM -MT "$*.o $@" $< > $@

-include $(DEPFILES)
