PREFIX = /usr/local

LIB_TESTS = 
LIBTREE_TESTS = root_node property_offset subnode_offset path_offset getprop \
	notfound \
	setprop_inplace nop_property nop_node
TESTS = $(LIB_TESTS) $(LIBTREE_TESTS)

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

all:	$(TESTS)

%.o: %.c
	@$(VECHO) CC $@
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ -c $<

%.o: %.S
	@$(VECHO) AS $@
	$(CC) -D__ASSEMBLY__ $(CPPFLAGS) -o $@ -c $<

$(LIB_TESTS): %: %.o testutils.o $(LIBFDT)
	@$(VECHO) LD "(testcase)" $@
	$(CC) $(LDFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(LIBTREE_TESTS): %: %.o testutils.o trees.o $(LIBFDT)
	@$(VECHO) LD "(testcase + trees)" $@
	$(CC) $(LDFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

clean:
	@$(VECHO) CLEAN "(tests)"
	rm -f *~ *.o *.so *.a *.d *.s core a.out
	rm -f $(TESTS)

%.d: %.c
	@$(CC) $(CPPFLAGS) -MM -MT "$*.o $@" $< > $@

-include $(DEPFILES)
