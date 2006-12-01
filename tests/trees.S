#include <fdt.h>
#include "testdata.h"

#define TREE_HDR(tree) \
	.globl	_##tree		; \
_##tree:	\
tree:	\
	.long	FDT_MAGIC	; \
	.long	tree##_end - tree ; \
	.long	tree##_struct - tree ; \
	.long	tree##_strings - tree ; \
	.long	tree##_rsvmap - tree ; \
	.long	0x11		; \
	.long	0x10		; \
	.long	0		; \
	.long	tree##_end - tree##_strings ; \
	.long	tree##_strings - tree##_struct ;

#define RSVMAP_ENTRY(addr, len) \
	.quad	addr		; \
	.quad	len		;

#define PROPHDR(tree, name, len) \
	.long	FDT_PROP	; \
	.long	tree##_##name - tree##_strings ; \
	.long	len		;

#define PROP_INT(tree, name, val) \
	PROPHDR(tree, name, 4) \
	.long	val

#define PROP_STR(tree, name, str) \
	PROPHDR(tree, name, 55f - 54f) \
54:	\
	.string	str		; \
55:	\
	.balign	4

#define BEGIN_NODE(name) \
	.long	FDT_BEGIN_NODE	; \
	.string	name		; \
	.balign 4

#define END_NODE \
	.long	FDT_END_NODE	;

#define STRING(tree, name, str) \
tree##_##name:	\
	.string	str
	
	.data

	TREE_HDR(test_tree1)

test_tree1_rsvmap:
	RSVMAP_ENTRY(0, 0)

test_tree1_struct:
	BEGIN_NODE("")
	PROP_INT(test_tree1, prop_int, TEST_VALUE_1)
	PROP_STR(test_tree1, prop_str, TEST_STRING_1)
	
	BEGIN_NODE("subnode1")
	PROP_INT(test_tree1, prop_int, TEST_VALUE_1)

	BEGIN_NODE("subsubnode")
	PROP_INT(test_tree1, prop_int, TEST_VALUE_1)
	END_NODE
	END_NODE

	BEGIN_NODE("subnode2")
	PROP_INT(test_tree1, prop_int, TEST_VALUE_2)

	BEGIN_NODE("subsubnode")
	PROP_INT(test_tree1, prop_int, TEST_VALUE_2)
	END_NODE
	END_NODE

	END_NODE
	.long	FDT_END

test_tree1_strings:
	STRING(test_tree1, prop_int, "prop-int")
	STRING(test_tree1, prop_str, "prop-str")
test_tree1_end:
