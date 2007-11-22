/*
 * (C) Copyright David Gibson <dwg@au1.ibm.com>, IBM Corporation.  2007.
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
 *                                                                   USA
 */

#include "dtc.h"

#ifdef TRACE_CHECKS
#define TRACE(c, ...) \
	do { \
		fprintf(stderr, "=== %s: ", (c)->name); \
		fprintf(stderr, __VA_ARGS__); \
		fprintf(stderr, "\n"); \
	} while (0)
#else
#define TRACE(c, fmt, ...)	do { } while (0)
#endif

enum checklevel {
	IGNORE = 0,
	WARN = 1,
	ERROR = 2,
};

enum checkstatus {
	UNCHECKED = 0,
	PREREQ,
	PASSED,
	FAILED,
};

struct check;

typedef void (*tree_check_fn)(struct check *c, struct node *dt);
typedef void (*node_check_fn)(struct check *c, struct node *dt, struct node *node);
typedef void (*prop_check_fn)(struct check *c, struct node *dt,
			      struct node *node, struct property *prop);

struct check {
	const char *name;
	tree_check_fn tree_fn;
	node_check_fn node_fn;
	prop_check_fn prop_fn;
	void *data;
	enum checklevel level;
	enum checkstatus status;
	int inprogress;
	int num_prereqs;
	struct check **prereq;
};

#define CHECK(nm, tfn, nfn, pfn, d, lvl, ...) \
	static struct check *nm##_prereqs[] = { __VA_ARGS__ }; \
	static struct check nm = { \
		.name = #nm, \
		.tree_fn = (tfn), \
		.node_fn = (nfn), \
		.prop_fn = (pfn), \
		.data = (d), \
		.level = (lvl), \
		.status = UNCHECKED, \
		.num_prereqs = ARRAY_SIZE(nm##_prereqs), \
		.prereq = nm##_prereqs, \
	};

#define TREE_CHECK(nm, d, lvl, ...) \
	CHECK(nm, check_##nm, NULL, NULL, d, lvl, __VA_ARGS__)
#define NODE_CHECK(nm, d, lvl, ...) \
	CHECK(nm, NULL, check_##nm, NULL, d, lvl, __VA_ARGS__)
#define PROP_CHECK(nm, d, lvl, ...) \
	CHECK(nm, NULL, NULL, check_##nm, d, lvl, __VA_ARGS__)
#define BATCH_CHECK(nm, lvl, ...) \
	CHECK(nm, NULL, NULL, NULL, NULL, lvl, __VA_ARGS__)

static inline void check_msg(struct check *c, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);

	if ((c->level < WARN) || (c->level <= quiet))
		return; /* Suppress message */

	fprintf(stderr, "%s (%s): ",
		(c->level == ERROR) ? "ERROR" : "Warning", c->name);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
}

#define FAIL(c, fmt, ...) \
	do { \
		TRACE((c), "\t\tFAILED at %s:%d", __FILE__, __LINE__); \
		(c)->status = FAILED; \
		check_msg((c), fmt, __VA_ARGS__); \
	} while (0)

static void check_nodes_props(struct check *c, struct node *dt, struct node *node)
{
	struct node *child;
	struct property *prop;

	TRACE(c, "%s", node->fullpath);
	if (c->node_fn)
		c->node_fn(c, dt, node);

	if (c->prop_fn)
		for_each_property(node, prop) {
			TRACE(c, "%s\t'%s'", node->fullpath, prop->name);
			c->prop_fn(c, dt, node, prop);
		}

	for_each_child(node, child)
		check_nodes_props(c, dt, child);
}

static int run_check(struct check *c, struct node *dt)
{
	int error = 0;
	int i;

	assert(!c->inprogress);

	if (c->status != UNCHECKED)
		goto out;

	c->inprogress = 1;

	for (i = 0; i < c->num_prereqs; i++) {
		struct check *prq = c->prereq[i];
		error |= run_check(prq, dt);
		if (prq->status != PASSED) {
			c->status = PREREQ;
			check_msg(c, "Failed prerequisite '%s'",
				  c->prereq[i]->name);
		}
	}

	if (c->status != UNCHECKED)
		goto out;

	if (c->node_fn || c->prop_fn)
		check_nodes_props(c, dt, dt);

	if (c->tree_fn)
		c->tree_fn(c, dt);
	if (c->status == UNCHECKED)
		c->status = PASSED;

	TRACE(c, "\tCompleted, status %d", c->status);

out:
	c->inprogress = 0;
	if ((c->status != PASSED) && (c->level == ERROR))
		error = 1;
	return error;
}

/*
 * Structural check functions
 */

static void check_duplicate_node_names(struct check *c, struct node *dt,
				       struct node *node)
{
	struct node *child, *child2;

	for_each_child(node, child)
		for (child2 = child->next_sibling;
		     child2;
		     child2 = child2->next_sibling)
			if (streq(child->name, child2->name))
				FAIL(c, "Duplicate node name %s",
				     child->fullpath);
}
NODE_CHECK(duplicate_node_names, NULL, ERROR);

static void check_duplicate_property_names(struct check *c, struct node *dt,
					   struct node *node)
{
	struct property *prop, *prop2;

	for_each_property(node, prop)
		for (prop2 = prop->next; prop2; prop2 = prop2->next)
			if (streq(prop->name, prop2->name))
				FAIL(c, "Duplicate property name %s in %s",
				     prop->name, node->fullpath);
}
NODE_CHECK(duplicate_property_names, NULL, ERROR);

static void check_explicit_phandles(struct check *c, struct node *root,
					  struct node *node)
{
	struct property *prop;
	struct node *other;
	cell_t phandle;

	prop = get_property(node, "linux,phandle");
	if (! prop)
		return; /* No phandle, that's fine */

	if (prop->val.len != sizeof(cell_t)) {
		FAIL(c, "%s has bad length (%d) linux,phandle property",
		     node->fullpath, prop->val.len);
		return;
	}

	phandle = propval_cell(prop);
	if ((phandle == 0) || (phandle == -1)) {
		FAIL(c, "%s has invalid linux,phandle value 0x%x",
		     node->fullpath, phandle);
		return;
	}

	other = get_node_by_phandle(root, phandle);
	if (other) {
		FAIL(c, "%s has duplicated phandle 0x%x (seen before at %s)",
		     node->fullpath, phandle, other->fullpath);
		return;
	}

	node->phandle = phandle;
}
NODE_CHECK(explicit_phandles, NULL, ERROR);

/*
 * Reference fixup functions
 */

static void fixup_phandle_references(struct check *c, struct node *dt,
				     struct node *node, struct property *prop)
{
      struct marker *m = prop->val.markers;
      struct node *refnode;
      cell_t phandle;

      for_each_marker_of_type(m, REF_PHANDLE) {
	      assert(m->offset + sizeof(cell_t) <= prop->val.len);

	      refnode = get_node_by_ref(dt, m->ref);
	      if (! refnode) {
		      FAIL(c, "Reference to non-existent node or label \"%s\"\n",
			   m->ref);
		      continue;
	      }

	      phandle = get_node_phandle(dt, refnode);
	      *((cell_t *)(prop->val.val + m->offset)) = cpu_to_be32(phandle);
      }
}
CHECK(phandle_references, NULL, NULL, fixup_phandle_references, NULL, ERROR,
      &duplicate_node_names, &explicit_phandles);

static struct check *check_table[] = {
	&duplicate_node_names, &duplicate_property_names,
	&explicit_phandles,
	&phandle_references,
};

void process_checks(int force, struct node *dt)
{
	int i;
	int error = 0;

	for (i = 0; i < ARRAY_SIZE(check_table); i++) {
		struct check *c = check_table[i];

		if (c->level != IGNORE)
			error = error || run_check(c, dt);
	}

	if (error) {
		if (!force) {
			fprintf(stderr, "ERROR: Input tree has errors, aborting "
				"(use -f to force output)\n");
			exit(2);
		} else if (quiet < 3) {
			fprintf(stderr, "Warning: Input tree has errors, "
				"output forced\n");
		}
	}
}

/*
 * Semantic check functions
 */

#define ERRMSG(...) if (quiet < 2) fprintf(stderr, "ERROR: " __VA_ARGS__)
#define WARNMSG(...) if (quiet < 1) fprintf(stderr, "Warning: " __VA_ARGS__)

#define DO_ERR(...) do {ERRMSG(__VA_ARGS__); ok = 0; } while (0)

static int must_be_one_cell(struct property *prop, struct node *node)
{
	if (prop->val.len != sizeof(cell_t)) {
		ERRMSG("\"%s\" property in %s has the wrong length (should be 1 cell)\n",
		       prop->name, node->fullpath);
		return 0;
	}

	return 1;
}

static int must_be_cells(struct property *prop, struct node *node)
{
	if ((prop->val.len % sizeof(cell_t)) != 0) {
		ERRMSG("\"%s\" property in %s is not a multiple of cell size\n",
		       prop->name, node->fullpath);
		return 0;
	}

	return 1;
}

static int must_be_string(struct property *prop, struct node *node)
{
	if (! data_is_one_string(prop->val)) {
		ERRMSG("\"%s\" property in %s is not a string\n",
		       prop->name, node->fullpath);
		return 0;
	}

	return 1;
}

static int name_prop_check(struct property *prop, struct node *node)
{
	if ((prop->val.len != node->basenamelen+1)
	    || !strneq(prop->val.val, node->name, node->basenamelen)) {
		ERRMSG("name property \"%s\" does not match node basename in %s\n",
		       prop->val.val,
		       node->fullpath);
		return 0;
	}

	return 1;
}

static struct {
	char *propname;
	int (*check_fn)(struct property *prop, struct node *node);
} prop_checker_table[] = {
	{"name", must_be_string},
	{"name", name_prop_check},
	{"linux,phandle", must_be_one_cell},
	{"#address-cells", must_be_one_cell},
	{"#size-cells", must_be_one_cell},
	{"reg", must_be_cells},
	{"model", must_be_string},
	{"device_type", must_be_string},
};

static int check_properties(struct node *node)
{
	struct property *prop;
	struct node *child;
	int i;
	int ok = 1;

	for_each_property(node, prop)
		for (i = 0; i < ARRAY_SIZE(prop_checker_table); i++)
			if (streq(prop->name, prop_checker_table[i].propname))
				if (! prop_checker_table[i].check_fn(prop, node)) {
					ok = 0;
					break;
				}

	for_each_child(node, child)
		if (! check_properties(child))
			ok = 0;

	return ok;
}

#define CHECK_HAVE(node, propname) \
	do { \
		if (! (prop = get_property((node), (propname)))) \
			DO_ERR("Missing \"%s\" property in %s\n", (propname), \
				(node)->fullpath); \
	} while (0);

#define CHECK_HAVE_WARN(node, propname) \
	do { \
		if (! (prop  = get_property((node), (propname)))) \
			WARNMSG("%s has no \"%s\" property\n", \
				(node)->fullpath, (propname)); \
	} while (0)

#define CHECK_HAVE_STRING(node, propname) \
	do { \
		CHECK_HAVE((node), (propname)); \
		if (prop && !data_is_one_string(prop->val)) \
			DO_ERR("\"%s\" property in %s is not a string\n", \
				(propname), (node)->fullpath); \
	} while (0)

#define CHECK_HAVE_STREQ(node, propname, value) \
	do { \
		CHECK_HAVE_STRING((node), (propname)); \
		if (prop && !streq(prop->val.val, (value))) \
			DO_ERR("%s has wrong %s, %s (should be %s\n", \
				(node)->fullpath, (propname), \
				prop->val.val, (value)); \
	} while (0)

#define CHECK_HAVE_ONECELL(node, propname) \
	do { \
		CHECK_HAVE((node), (propname)); \
		if (prop && (prop->val.len != sizeof(cell_t))) \
			DO_ERR("\"%s\" property in %s has wrong size %d (should be 1 cell)\n", (propname), (node)->fullpath, prop->val.len); \
	} while (0)

#define CHECK_HAVE_WARN_ONECELL(node, propname) \
	do { \
		CHECK_HAVE_WARN((node), (propname)); \
		if (prop && (prop->val.len != sizeof(cell_t))) \
			DO_ERR("\"%s\" property in %s has wrong size %d (should be 1 cell)\n", (propname), (node)->fullpath, prop->val.len); \
	} while (0)

#define CHECK_HAVE_WARN_PHANDLE(xnode, propname, root) \
	do { \
		struct node *ref; \
		CHECK_HAVE_WARN_ONECELL((xnode), (propname)); \
		if (prop) {\
			cell_t phandle = propval_cell(prop); \
			if ((phandle == 0) || (phandle == -1)) { \
				DO_ERR("\"%s\" property in %s contains an invalid phandle %x\n", (propname), (xnode)->fullpath, phandle); \
			} else { \
				ref = get_node_by_phandle((root), propval_cell(prop)); \
				if (! ref) \
					DO_ERR("\"%s\" property in %s refers to non-existant phandle %x\n", (propname), (xnode)->fullpath, propval_cell(prop)); \
			} \
		} \
	} while (0)

#define CHECK_HAVE_WARN_STRING(node, propname) \
	do { \
		CHECK_HAVE_WARN((node), (propname)); \
		if (prop && !data_is_one_string(prop->val)) \
			DO_ERR("\"%s\" property in %s is not a string\n", \
				(propname), (node)->fullpath); \
	} while (0)

static int check_root(struct node *root)
{
	struct property *prop;
	int ok = 1;

	CHECK_HAVE_STRING(root, "model");

	CHECK_HAVE(root, "#address-cells");
	CHECK_HAVE(root, "#size-cells");

	CHECK_HAVE_WARN(root, "compatible");

	return ok;
}

static int check_cpus(struct node *root, int outversion, int boot_cpuid_phys)
{
	struct node *cpus, *cpu;
	struct property *prop;
	struct node *bootcpu = NULL;
	int ok = 1;

	cpus = get_subnode(root, "cpus");
	if (! cpus) {
		ERRMSG("Missing /cpus node\n");
		return 0;
	}

	CHECK_HAVE_WARN(cpus, "#address-cells");
	CHECK_HAVE_WARN(cpus, "#size-cells");

	for_each_child(cpus, cpu) {
		CHECK_HAVE_STREQ(cpu, "device_type", "cpu");

		if (cpu->addr_cells != 1)
			DO_ERR("%s has bad #address-cells value %d (should be 1)\n",
			       cpu->fullpath, cpu->addr_cells);
		if (cpu->size_cells != 0)
			DO_ERR("%s has bad #size-cells value %d (should be 0)\n",
			       cpu->fullpath, cpu->size_cells);

		CHECK_HAVE_ONECELL(cpu, "reg");
		if (prop) {
			cell_t unitnum;
			char *eptr;

			unitnum = strtol(get_unitname(cpu), &eptr, 16);
			if (*eptr) {
				WARNMSG("%s has bad format unit name %s (should be CPU number\n",
					cpu->fullpath, get_unitname(cpu));
			} else if (unitnum != propval_cell(prop)) {
				WARNMSG("%s unit name \"%s\" does not match \"reg\" property <%x>\n",
				       cpu->fullpath, get_unitname(cpu),
				       propval_cell(prop));
			}
		}

/* 		CHECK_HAVE_ONECELL(cpu, "d-cache-line-size"); */
/* 		CHECK_HAVE_ONECELL(cpu, "i-cache-line-size"); */
		CHECK_HAVE_ONECELL(cpu, "d-cache-size");
		CHECK_HAVE_ONECELL(cpu, "i-cache-size");

		CHECK_HAVE_WARN_ONECELL(cpu, "clock-frequency");
		CHECK_HAVE_WARN_ONECELL(cpu, "timebase-frequency");

		prop = get_property(cpu, "linux,boot-cpu");
		if (prop) {
			if (prop->val.len)
				WARNMSG("\"linux,boot-cpu\" property in %s is non-empty\n",
					cpu->fullpath);
			if (bootcpu)
				DO_ERR("Multiple boot cpus (%s and %s)\n",
				       bootcpu->fullpath, cpu->fullpath);
			else
				bootcpu = cpu;
		}
	}

	if (outversion < 2) {
		if (! bootcpu)
			WARNMSG("No cpu has \"linux,boot-cpu\" property\n");
	} else {
		if (bootcpu)
			WARNMSG("\"linux,boot-cpu\" property is deprecated in blob version 2 or higher\n");
		if (boot_cpuid_phys == 0xfeedbeef)
			WARNMSG("physical boot CPU not set.  Use -b option to set\n");
	}

	return ok;
}

static int check_memory(struct node *root)
{
	struct node *mem;
	struct property *prop;
	int nnodes = 0;
	int ok = 1;

	for_each_child(root, mem) {
		if (! strneq(mem->name, "memory", mem->basenamelen))
			continue;

		nnodes++;

		CHECK_HAVE_STREQ(mem, "device_type", "memory");
		CHECK_HAVE(mem, "reg");
	}

	if (nnodes == 0) {
		ERRMSG("No memory nodes\n");
		return 0;
	}

	return ok;
}

static int check_chosen(struct node *root)
{
	struct node *chosen;
	struct property *prop;
	int ok = 1;

	chosen = get_subnode(root, "chosen");
	if (!chosen)
		return ok;

        /* give warning for obsolete interrupt-controller property */
	do {
		if ((prop = get_property(chosen, "interrupt-controller")) != NULL) {
			WARNMSG("%s has obsolete \"%s\" property\n",
                                 chosen->fullpath, "interrupt-controller");
                }
	} while (0);

	return ok;
}

static int check_addr_size_reg(struct node *node,
			       int p_addr_cells, int p_size_cells)
{
	int addr_cells = p_addr_cells;
	int size_cells = p_size_cells;
	struct property *prop;
	struct node *child;
	int ok = 1;

	node->addr_cells = addr_cells;
	node->size_cells = size_cells;

	prop = get_property(node, "reg");
	if (prop) {
		int len = prop->val.len / 4;

		if ((len % (addr_cells+size_cells)) != 0)
			DO_ERR("\"reg\" property in %s has invalid length (%d) for given #address-cells (%d) and #size-cells (%d)\n",
			       node->fullpath, prop->val.len,
			       addr_cells, size_cells);
	}

	prop = get_property(node, "#address-cells");
	if (prop)
		addr_cells = propval_cell(prop);

	prop = get_property(node, "#size-cells");
	if (prop)
		size_cells = propval_cell(prop);

	for_each_child(node, child) {
		ok = ok && check_addr_size_reg(child, addr_cells, size_cells);
	}

	return ok;
}

int check_semantics(struct node *dt, int outversion, int boot_cpuid_phys)
{
	int ok = 1;

	ok = ok && check_properties(dt);
	ok = ok && check_addr_size_reg(dt, -1, -1);
	ok = ok && check_root(dt);
	ok = ok && check_cpus(dt, outversion, boot_cpuid_phys);
	ok = ok && check_memory(dt);
	ok = ok && check_chosen(dt);
	if (! ok)
		return 0;

	return 1;
}
