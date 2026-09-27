/*
 * Copyright (C) 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef LIBUFDT_H
#define LIBUFDT_H

#include "libufdt_sysdeps.h"
#include "ufdt_types.h"

/*
 * BEGIN of ufdt_node methods
 */

/*
 * Allocates spaces for new ufdt_node who represents a fdt node at fdt_tag_ptr.
 * In order to get name pointer, it's necessary to give the pointer to the
 * entire fdt it belongs to.
 *
 *
 * @return: a pointer to the newly created ufdt_node or
 *          NULL if dto_malloc failed
 */
struct ufdt_node *ufdt_node_construct(void *fdtp, fdt32_t *fdt_tag_ptr,
                                      struct ufdt_node_pool *pool);

/*
 * Frees all nodes in the subtree rooted at *node.
 */
void ufdt_node_destruct(struct ufdt_node *node, struct ufdt_node_pool *pool);

/*
 * Adds the child as a subnode of the parent.
 * It's been done by add entries in parent->prop_list or node_list depending on
 * the tag type of child.
 *
 * @return: 0 if success
 *          < 0 otherwise
 *
 * @Time: O(1) w.h.p.
 */
int ufdt_node_add_child(struct ufdt_node *parent, struct ufdt_node *child);

/* BEGIN of FDT_PROP related functions .*/

/*
 * Gets pointer to FDT_PROP subnode of node with name equals to name[0..len-1]
 *
 * @return: a pointer to the subnode or
 *          NULL if no such subnode.
 *
 * @Time: O(len = length of name) w.h.p.
 */
struct ufdt_node *ufdt_node_get_property_by_name_len(
    const struct ufdt_node *node, const char *name, int len);
struct ufdt_node *ufdt_node_get_property_by_name(const struct ufdt_node *node,
                                                 const char *name);

/*
 * Gets the pointer to the FDT_PROP node's data in the corresponding fdt.
 * Also writes the length of such data to *out_len if out_len is not NULL.
 *
 * @return: a pointer to some data located in fdt or
 *          NULL if *node is not a FDT_PROP
 */
char *ufdt_node_get_fdt_prop_data(const struct ufdt_node *node, int *out_len);

/*
 * Gets pointer to FDT_PROP node's data in fdt with name equals to
 * name[0..len-1], which is a subnode of *node.
 * It's actually a composition of ufdt_node_get_property_by_name and
 * ufdt_node_get_fdt_prop_data
 *
 * @return: a pointer to some data located in fdt or
 *          NULL if no such subnode.
 *
 * @Time: O(len = length of name) w.h.p.
 */
char *ufdt_node_get_fdt_prop_data_by_name_len(const struct ufdt_node *node,
                                              const char *name, int len,
                                              int *out_len);
char *ufdt_node_get_fdt_prop_data_by_name(const struct ufdt_node *node,
                                          const char *name, int *out_len);

/* END of FDT_PROP related functions .*/

/*
 * Gets pointer to FDT_BEGIN_NODE subnode of node with name equals to
 * name[0..len-1].
 *
 * @return: a pointer to the subnode or
 *          NULL if no such subnode.
 *
 * @Time: O(len = length of name) w.h.p.
 */

struct ufdt_node *ufdt_node_get_subnode_by_name_len(const struct ufdt_node *node,
                                                    const char *name, int len);
struct ufdt_node *ufdt_node_get_subnode_by_name(const struct ufdt_node *node,
                                                const char *name);

/*
 * Gets the pointer to FDT_NODE node whose relative path to *node is
 * path[0..len-1].
 * Note that the relative path doesn't support parent node like:
 * "../path/to/node".
 *
 * @return: a pointer to the node or
 *          NULL if no such node.
 *
 * @Time: O(len = length of path) w.h.p.
 */
struct ufdt_node *ufdt_node_get_node_by_path_len(const struct ufdt_node *node,
                                                 const char *path, int len);
struct ufdt_node *ufdt_node_get_node_by_path(const struct ufdt_node *node,
                                             const char *path);

/*
 * Gets the phandle value of the node if it has.
 *
 * @return: phandle value of that node or
 *          0 if *node is not FDT_NODE or there's no "phandle"/"linux,phandle"
 *          property.
 *
 * @Time: O(1) w.h.p.
 */
uint32_t ufdt_node_get_phandle(const struct ufdt_node *node);

/*
 * END of ufdt_node methods
 */

/*
 * BEGIN of ufdt methods.
 */

/*
 * Constructs a ufdt whose base fdt is fdtp.
 * Note that this function doesn't construct the entire tree.
 * To get the whole tree please call `ufdt_from_fdt(fdtp, fdt_size)`
 *
 * @return: an empty ufdt with base fdtp = fdtp
 */
struct ufdt *ufdt_construct(void *fdtp, struct ufdt_node_pool *pool);

/*
 * Frees the space occupied by the ufdt, including all ufdt_nodes
 * with ufdt_static_phandle_table.
 */
void ufdt_destruct(struct ufdt *tree, struct ufdt_node_pool *pool);

/*
 * Add a fdt into this ufdt.
 * Note that this function just add the given fdtp into this ufdt,
 * and doesn't create any node.
 *
 * @return: 0 if success.
 */
int ufdt_add_fdt(struct ufdt *tree, void *fdtp);

/*
 * Calculate the offset in the string tables of the given string.
 * All string tables will be concatenated in reversed order.
 *
 * @return: The offset is a negative number, base on the end position of
 *          all concatenated string tables
 *          Return 0 if not in any string table.
 */
int ufdt_get_string_off(const struct ufdt *tree, const char *s);

/*
 * Gets the pointer to the ufdt_node in tree with phandle = phandle.
 * The function do a binary search in tree->phandle_table.
 *
 * @return: a pointer to the target ufdt_node
 *          NULL if no ufdt_node has phandle = phandle
 *
 * @Time: O(log(# of nodes in tree)) = O(log(size of underlying fdt))
 */
struct ufdt_node *ufdt_get_node_by_phandle(struct ufdt *tree, uint32_t phandle);

/*
 * Gets the pointer to the ufdt_node in tree with absolute path =
 * path[0..len-1].
 * Absolute path has form "/path/to/node" or "some_alias/to/node".
 * In later example, some_alias is a property in "/aliases" with data is a path
 * to some node X. Then the funcion will return node with relative
 * path = "to/node" w.r.t. X.
 *
 * @return: a pointer to the target ufdt_node or
 *          NULL if such dnt doesn't exist.
 *
 * @Time: O(len = length of path) w.h.p.
 */
struct ufdt_node *ufdt_get_node_by_path_len(struct ufdt *tree, const char *path,
                                            int len);
struct ufdt_node *ufdt_get_node_by_path(struct ufdt *tree, const char *path);

/*
 * Determines whether node->name equals to name[0..len-1]
 *
 * @return: true if they're equal.
 *          false otherwise
 */
bool ufdt_node_name_eq(const struct ufdt_node *node, const char *name, int len);

/*
 * Merges tree_b into tree_a with tree_b has all nodes except root disappeared.
 * Overwrite property in tree_a if there's one with same name in tree_b.
 * Otherwise add the property to tree_a.
 * For subnodes with the same name, recursively run this function.
 *
 * Ex:
 * tree_a : ta {
 *  b = "b";
 *  c = "c";
 *  d {
 *    e = "g";
 *  };
 * };
 *
 * tree_b : tb {
 *  c = "C";
 *  g = "G";
 *  d {
 *    da = "dad";
 *  };
 *  h {
 *    hh = "HH";
 *  };
 * };
 *
 * The resulting trees will be:
 *
 * tree_a : ta {
 *  b = "b";
 *  c = "C";
 *  g = "G";
 *  d {
 *    da = "dad";
 *    e = "g";
 *  };
 *  h {
 *    hh = "HH";
 *  };
 * };
 *
 * tree_b : tb {
 * };
 *
 *
 * @return: 0 if merge success
 *          < 0 otherwise
 *
 * @Time: O(# of nodes in tree_b + total length of all names in tree_b) w.h.p.
 */
int ufdt_node_merge_into(struct ufdt_node *node_a, struct ufdt_node *node_b,
                         struct ufdt_node_pool *pool);

/*
 * END of ufdt methods.
 */

/*
 * BEGIN of ufdt output functions
 */

/*
 * Builds the ufdt for FDT pointed by fdtp.
 *
 * @return: the ufdt T representing fdtp or
 *          T with T.fdtp == NULL if fdtp is unvalid.
 *
 * @Time: O(fdt_size + nlogn) where n = # of nodes in fdt.
 */
struct ufdt *ufdt_from_fdt(void *fdtp, size_t fdt_size,
                           struct ufdt_node_pool *pool);

/*
 * Sequentially dumps the whole ufdt to FDT buffer fdtp with buffer size
 * buf_size.
 *
 * Basically using functions provided by libfdt/fdt_sw.c.
 *
 * @return: 0 if successfully dump or
 *          < 0 otherwise
 *
 * @Time: O(total length of all names + # of nodes in tree)
 */
int ufdt_to_fdt(const struct ufdt *tree, void *buf, int buf_size);

/*
 * prints the entire subtree rooted at *node in form:
 * NODE :[node name]:
 *   PROP :[prop name]:
 *   ...
 *   NODE :[subnode1 name]:
 *     ...
 *   NODE :[subnode1 name]:
 *     ...
 *   ...
 * There're (depth * TAB_SIZE) spaces in front of each line.
 */
void ufdt_node_print(const struct ufdt_node *node, int depth);

/*
 * It's just ufdt_node_print(tree->root, 0).
 */
void ufdt_print(struct ufdt *tree);

/*
 * END of ufdt output functions
 */

#endif /* LIBUFDT_H */
