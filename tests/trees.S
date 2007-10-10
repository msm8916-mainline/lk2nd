#include <fdt.h>
#include "testdata.h"

#define FDTLONG(val) \
	.byte	((val) >> 24) & 0xff ; \
	.byte	((val) >> 16) & 0xff ; \
	.byte	((val) >> 8) & 0xff ; \
	.byte	(val) & 0xff

#define FDTQUAD(val) \
	.byte	((val) >> 56) & 0xff ; \
	.byte	((val) >> 48) & 0xff ; \
	.byte	((val) >> 40) & 0xff ; \
	.byte	((val) >> 32) & 0xff ; \
	.byte	((val) >> 24) & 0xff ; \
	.byte	((val) >> 16) & 0xff ; \
	.byte	((val) >> 8) & 0xff ; \
	.byte	(val) & 0xff

#define TREE_HDR(tree) \
	.balign	4		; \
	.globl	_##tree		; \
_##tree:	\
tree:	\
	FDTLONG(FDT_MAGIC)	; \
	FDTLONG(tree##_end - tree) ; \
	FDTLONG(tree##_struct - tree) ; \
	FDTLONG(tree##_strings - tree) ; \
	FDTLONG(tree##_rsvmap - tree) ; \
	FDTLONG(0x11)		; \
	FDTLONG(0x10)		; \
	FDTLONG(0)		; \
	FDTLONG(tree##_end - tree##_strings) ; \
	FDTLONG(tree##_strings - tree##_struct) ;

#define RSVMAP_ENTRY(addr, len) \
	FDTQUAD(addr)		; \
	FDTQUAD(len)		;

#define EMPTY_RSVMAP(tree) \
	.balign	8		; \
tree##_rsvmap:			; \
	RSVMAP_ENTRY(0, 0)

#define PROPHDR(tree, name, len) \
	FDTLONG(FDT_PROP)	; \
	FDTLONG(len)		; \
	FDTLONG(tree##_##name - tree##_strings) ;

#define PROP_INT(tree, name, val) \
	PROPHDR(tree, name, 4) \
	/* For ease of testing the property values go in native-endian */ \
	.long	val

#define PROP_STR(tree, name, str) \
	PROPHDR(tree, name, 55f - 54f) \
54:	\
	.string	str		; \
55:	\
	.balign	4

#define BEGIN_NODE(name) \
	FDTLONG(FDT_BEGIN_NODE)	; \
	.string	name		; \
	.balign 4

#define END_NODE \
	FDTLONG(FDT_END_NODE)	;

#define STRING(tree, name, str) \
tree##_##name:	\
	.string	str

	.data

	TREE_HDR(test_tree1)

	.balign	8
test_tree1_rsvmap:
	RSVMAP_ENTRY(TEST_ADDR_1, TEST_SIZE_1)
	RSVMAP_ENTRY(TEST_ADDR_2, TEST_SIZE_2)
	RSVMAP_ENTRY(0, 0)

test_tree1_struct:
	BEGIN_NODE("")
	PROP_INT(test_tree1, prop_int, TEST_VALUE_1)
	PROP_STR(test_tree1, prop_str, TEST_STRING_1)

	BEGIN_NODE("subnode@1")
	PROP_INT(test_tree1, prop_int, TEST_VALUE_1)

	BEGIN_NODE("subsubnode")
	PROP_INT(test_tree1, prop_int, TEST_VALUE_1)
	END_NODE
	END_NODE

	BEGIN_NODE("subnode@2")
	PROP_INT(test_tree1, prop_int, TEST_VALUE_2)

	BEGIN_NODE("subsubnode@0")
	PROP_INT(test_tree1, prop_int, TEST_VALUE_2)
	END_NODE
	END_NODE

	END_NODE
	FDTLONG(FDT_END)

test_tree1_strings:
	STRING(test_tree1, prop_int, "prop-int")
	STRING(test_tree1, prop_str, "prop-str")
test_tree1_end:

	TREE_HDR(truncated_property)
	EMPTY_RSVMAP(truncated_property)

truncated_property_struct:
	BEGIN_NODE("")
	PROPHDR(truncated_property, prop_truncated, 4)
	/* Oops, no actual property data here */

truncated_property_strings:
	STRING(truncated_property, prop_truncated, "truncated")
truncated_property_end:
