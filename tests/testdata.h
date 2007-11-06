#include <endian.h>

#if __BYTE_ORDER == __BIG_ENDIAN
#define cell_to_fdt(x)	(x)
#else
/* We do this as a big hairy expression instead of using bswap_32()
 * because we need it to work in asm as well as C. */
#define cell_to_fdt(x)	((((x) >> 24) & 0xff) | (((x) >> 8) & 0xff00) \
			 | (((x) << 8) & 0xff0000) | (((x) << 24) & 0xff000000))
#endif

#ifdef __ASSEMBLY__
#define ASM_CONST_LL(x)	(x)
#else
#define ASM_CONST_LL(x)	(x##ULL)
#endif

#define TEST_ADDR_1	ASM_CONST_LL(0xdeadbeef00000000)
#define TEST_SIZE_1	ASM_CONST_LL(0x100000)
#define TEST_ADDR_2	ASM_CONST_LL(123456789)
#define TEST_SIZE_2	ASM_CONST_LL(010000)

#define TEST_VALUE_1	cell_to_fdt(0xdeadbeef)
#define TEST_VALUE_2	cell_to_fdt(123456789)

#define TEST_STRING_1	"hello world"
#define TEST_STRING_2	"nastystring: \a\b\t\n\v\f\r\\\""
#define TEST_STRING_3	"\xde\xad\xbe\xef"

#ifndef __ASSEMBLY__
extern struct fdt_header _test_tree1;
extern struct fdt_header _truncated_property;
#endif /* ! __ASSEMBLY */
