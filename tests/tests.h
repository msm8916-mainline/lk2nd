#ifndef _TESTS_H
#define _TESTS_H
/*
 * libfdt - Flat Device Tree manipulation
 *	Testcase definitions
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#define DEBUG

/* Test return codes */
#define RC_PASS 	0
#define RC_CONFIG 	1
#define RC_FAIL		2
#define RC_BUG		99

extern int verbose_test;
extern char *test_name;
void test_init(int argc, char *argv[]);

#define ALIGN(x, a)	(((x) + (a) - 1) & ~((a) - 1))
#define PALIGN(p, a)	((void *)ALIGN((unsigned long)(p), (a)))

#define streq(s1, s2)	(strcmp((s1),(s2)) == 0)

#if __BYTE_ORDER == __BIG_ENDIAN
#define fdt32_to_cpu(x)		(x)
#define cpu_to_fdt32(x)		(x)
#define fdt64_to_cpu(x)		(x)
#define cpu_to_fdt64(x)		(x)
#else
#define fdt32_to_cpu(x)		(bswap_32((x)))
#define cpu_to_fdt32(x)		(bswap_32((x)))
#define fdt64_to_cpu(x)		(bswap_64((x)))
#define cpu_to_fdt64(x)		(bswap_64((x)))
#endif

/* Each test case must define this function */
void cleanup(void);

#define verbose_printf(...) \
	if (verbose_test) { \
		printf(__VA_ARGS__); \
		fflush(stdout); \
	}
#define ERR	"ERR: "
#define ERROR(fmt, args...)	fprintf(stderr, ERR fmt, ## args)


#define	PASS()						\
	do {						\
		cleanup();				\
		printf("PASS\n");			\
		exit(RC_PASS);				\
	} while (0)

#define	PASS_INCONCLUSIVE()				\
	do {						\
		cleanup();				\
		printf("PASS (inconclusive)\n");	\
		exit(RC_PASS);				\
	} while (0)

#define IRRELEVANT()					\
	do {						\
		cleanup();				\
		printf("PASS (irrelevant)\n");		\
		exit(RC_PASS);				\
	} while (0)

/* Look out, gcc extension below... */
#define FAIL(fmt, ...)					\
	do {						\
		cleanup();				\
		printf("FAIL\t" fmt "\n", ##__VA_ARGS__);	\
		exit(RC_FAIL);				\
	} while (0)

#define CONFIG(fmt, ...)				\
	do {						\
		cleanup();				\
		printf("Bad configuration: " fmt "\n", ##__VA_ARGS__);	\
		exit(RC_CONFIG);			\
	} while (0)

#define TEST_BUG(fmt, ...)				\
	do {						\
		cleanup();				\
		printf("BUG in testsuite: " fmt "\n", ##__VA_ARGS__);	\
		exit(RC_BUG);				\
	} while (0)

static inline void *xmalloc(size_t size)
{
	void *p = malloc(size);
	if (! p)
		FAIL("malloc() failure");
	return p;
}

static inline void *xrealloc(void *p, size_t size)
{
	p = realloc(p, size);
	if (! p)
		FAIL("realloc() failure");
	return p;
}

const char *fdt_strerror(int errval);
void check_property(struct fdt_header *fdt, int nodeoffset, const char *name,
		    int len, const void *val);
#define check_property_typed(fdt, nodeoffset, name, val) \
	({ \
		typeof(val) x = val; \
		check_property(fdt, nodeoffset, name, sizeof(x), &x); \
	})


void *check_getprop(struct fdt_header *fdt, int nodeoffset, const char *name,
		    int len, const void *val);
#define check_getprop_typed(fdt, nodeoffset, name, val) \
	({ \
		typeof(val) x = val; \
		check_getprop(fdt, nodeoffset, name, sizeof(x), &x); \
	})
//void *load_blob(const char *filename);
void *load_blob_arg(int argc, char *argv[]);

#endif /* _TESTS_H */
