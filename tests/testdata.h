#include <endian.h>

#if __BYTE_ORDER == __BIG_ENDIAN
#define cell_to_fdt(x)	(x)
#else
/* We do this as a big hairy expression instead of using bswap_32()
 * because we need it to work in asm as well as C. */
#define cell_to_fdt(x)	((((x) >> 24) & 0xff) | (((x) >> 8) & 0xff00) \
			 | (((x) << 8) & 0xff0000) | (((x) << 24) & 0xff000000))
#endif

#define TEST_VALUE_1	cell_to_fdt(0xdeadbeef)
#define TEST_VALUE_2	cell_to_fdt(0xabcd1234)

#define TEST_STRING_1	"hello world"

#ifndef __ASSEMBLY__
extern struct fdt_header _test_tree1;
extern struct fdt_header _truncated_property;
#endif /* ! __ASSEMBLY */
