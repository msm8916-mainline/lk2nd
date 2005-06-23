#include <stdint.h>
#include <string.h>

#include "flat_dt.h"

typedef uint32_t u32;
typedef uint64_t u64;

#define ALIGN(x, a)	(((x) + ((a) - 1)) & ((a) - 1))
#define PALIGN(p, a)	((void *)(ALIGN((unsigned long)(p), (a))))

#define GET_CELL(p)	(p += 4, *((u32 *)(p-4)))

static char *skip_name(char *p)
{
	while (*p != '\0')
		p++;

	return PALIGN(p, sizeof(u32));
}

static char *skip_prop(void *blob, char *p)
{
	struct boot_param_header *bph = blob;
	u32 len, nameoff;

	len = GET_CELL(p);
	nameoff = GET_CELL(p);
	if ((bph->version < 0x10) && (len >= sizeof(u64)))
		p = PALIGN(p, sizeof(u64));
	return PALIGN(p + len, sizeof(u32));
}

static char *get_unit(char *dtpath)
{
	char *p;

	if (dtpath[0] != '/')
		return dtpath;

	p = dtpath + strlen(dtpath);
	while (*p != '/')
		p--;

	return p+1;
}

static int first_seg_len(char *dtpath)
{
	int len = 0;

	while ((dtpath[len] != '/') && (dtpath[len] != '\0'))
		len++;

	return len;
}

char *flat_dt_get_string(void *blob, u32 offset)
{
	struct boot_param_header *bph = blob;

	return (char *)blob + bph->off_dt_strings + offset;
}

void *flat_dt_get_subnode(void *blob, void *node, char *uname, int unamelen)
{
	struct boot_param_header *bph = blob;
	char *p = node;
	u32 tag;
	int depth = 0;
	char *nuname;

	if (! unamelen)
		unamelen = strlen(uname);

	do {
		tag = GET_CELL(p);

		switch (tag) {
		case OF_DT_PROP:
			p = skip_prop(blob, p);
			break;

		case OF_DT_BEGIN_NODE:
			if (depth == 0) {
				nuname = p;

				if (bph->version < 0x10)
					nuname = get_unit(nuname);

				p = skip_name(p);

				if (strncmp(nuname, uname, unamelen) == 0)
					return p;
			}
			depth++;
			break;

		case OF_DT_END_NODE:
			depth--;
			break;

		case OF_DT_END:
			/* looks like a malformed tree */
			return NULL;
			break;

		default:
			/* FIXME: throw some sort of error */
			return NULL;
		}
	} while (depth >= 0);

	return NULL;
}

void *flat_dt_get_node(void *blob, char *path)
{
	struct boot_param_header *bph = blob;
	char *node;
	int seglen;

	node = blob + bph->off_dt_struct;
	node += sizeof(u32); /* skip initial OF_DT_BEGIN_NODE */
	node = skip_name(node);	/* skip root node name */

	while (node && (*path)) {
		if (path[0] == '/')
			path++;

		seglen = first_seg_len(path);

		node = flat_dt_get_subnode(blob, node, path, seglen);

		path += seglen;
	}

	return node;
}

void flat_dt_traverse(void *blob, int (*fn)(void *blob, void *node, void *priv),
		      void *private)
{
	struct boot_param_header *bph = blob;
	char *p;
	u32 tag;
	int depth = 0;
	char *uname;

	p = (char *)blob + bph->off_dt_struct;

	tag = GET_CELL(p);
	while (tag != OF_DT_END) {
		switch (tag) {
		case OF_DT_BEGIN_NODE:
			uname = p;

			if (bph->version < 0x10)
				uname = get_unit(uname);

			p = skip_name(p);

			(*fn)(blob, p, private);
			depth++;
			break;

		case OF_DT_END_NODE:
			depth--;
			break;

		case OF_DT_PROP:
			p = skip_prop(blob, p);
			break;

		default:
			/* FIXME: badly formed tree */
			return;
		}
	}
}

void *flat_dt_get_prop(void *blob, void *node, char *name, u32 *len)
{
	struct boot_param_header *bph = blob;
	char *p = node;

	do {
		u32 tag = GET_CELL(p);
		u32 sz, noff;
		const char *nstr;

		if (tag != OF_DT_PROP)
			return NULL;

		sz = GET_CELL(p);
		noff = GET_CELL(p);

		/* Old versions have variable alignment of the
		 * property value */
		if ((bph->version < 0x10) && (sz >= 8))
		    p = PALIGN(p, 8);

		nstr = flat_dt_get_string(blob, noff);

		if (strcmp(name, nstr) == 0) {
			if (len)
				*len = sz;
			return (void *)p;
		}

		p = PALIGN(p + sz, sizeof(u32));
	} while(1);
}
