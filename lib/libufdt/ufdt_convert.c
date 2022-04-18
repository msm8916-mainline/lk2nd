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

#include "libufdt.h"

#include "ufdt_node_pool.h"
#include "ufdt_prop_dict.h"

struct ufdt *ufdt_construct(void *fdtp, struct ufdt_node_pool *pool) {
  (void)(pool); /* unused parameter */

  /* Inital size is 2, will be exponentially increased when it needed later.
     (2 -> 4 -> 8 -> ...) */
  const int DEFAULT_MEM_SIZE_FDTPS = 2;

  void **fdtps = NULL;
  struct ufdt *res_ufdt = NULL;

  fdtps = (void **)dto_malloc(sizeof(void *) * DEFAULT_MEM_SIZE_FDTPS);
  if (fdtps == NULL) goto error;
  fdtps[0] = fdtp;

  res_ufdt = dto_malloc(sizeof(struct ufdt));
  if (res_ufdt == NULL) goto error;

  res_ufdt->fdtps = fdtps;
  res_ufdt->mem_size_fdtps = DEFAULT_MEM_SIZE_FDTPS;
  res_ufdt->num_used_fdtps = (fdtp != NULL ? 1 : 0);
  res_ufdt->root = NULL;

  return res_ufdt;

error:
  if (res_ufdt) dto_free(res_ufdt);
  if (fdtps) dto_free(fdtps);

  return NULL;
}

void ufdt_destruct(struct ufdt *tree, struct ufdt_node_pool *pool) {
  if (tree == NULL) return;

  ufdt_node_destruct(tree->root, pool);

  dto_free(tree->fdtps);
  dto_free(tree->phandle_table.data);
  dto_free(tree);
}

int ufdt_add_fdt(struct ufdt *tree, void *fdtp) {
  if (fdtp == NULL) {
    return -1;
  }

  int i = tree->num_used_fdtps;
  if (i >= tree->mem_size_fdtps) {
    int new_size = tree->mem_size_fdtps * 2;
    void **new_fdtps = dto_malloc(sizeof(void *) * new_size);
    if (new_fdtps == NULL) return -1;

    dto_memcpy(new_fdtps, tree->fdtps, sizeof(void *) * tree->mem_size_fdtps);
    dto_free(tree->fdtps);

    tree->fdtps = new_fdtps;
    tree->mem_size_fdtps = new_size;
  }

  tree->fdtps[i] = fdtp;
  tree->num_used_fdtps = i + 1;

  return 0;
}

int ufdt_get_string_off(const struct ufdt *tree, const char *s) {
  /* fdt_create() sets the dt_string_off to the end of fdt buffer,
     and _ufdt_output_strtab_to_fdt() copy all string tables in reversed order.
     So, here the return offset value is base on the end of all string buffers,
     and it should be a minus value. */
  int res_off = 0;
  for (int i = 0; i < tree->num_used_fdtps; i++) {
    void *fdt = tree->fdtps[i];
    const char *strtab_start = (const char *)fdt + fdt_off_dt_strings(fdt);
    int strtab_size = fdt_size_dt_strings(fdt);
    const char *strtab_end = strtab_start + strtab_size;

    /* Check if the string is in the string table */
    if (s >= strtab_start && s < strtab_end) {
      res_off += (s - strtab_end);
      return res_off;
    }

    res_off -= strtab_size;
  }
  /* Can not find the string, return 0 */
  return 0;
}

static struct ufdt_node *ufdt_new_node(void *fdtp, int node_offset,
                                       struct ufdt_node_pool *pool) {
  if (fdtp == NULL) {
    dto_error("Failed to get new_node because tree is NULL\n");
    return NULL;
  }

  fdt32_t *fdt_tag_ptr =
      (fdt32_t *)fdt_offset_ptr(fdtp, node_offset, sizeof(fdt32_t));
  struct ufdt_node *res = ufdt_node_construct(fdtp, fdt_tag_ptr, pool);
  return res;
}

static struct ufdt_node *fdt_to_ufdt_tree(void *fdtp, int cur_fdt_tag_offset,
                                          int *next_fdt_tag_offset, int cur_tag,
                                          struct ufdt_node_pool *pool) {
  if (fdtp == NULL) {
    return NULL;
  }
  uint32_t tag;
  struct ufdt_node *res, *child_node;

  res = NULL;
  child_node = NULL;
  tag = cur_tag;

  switch (tag) {
    case FDT_END_NODE:
    case FDT_NOP:
    case FDT_END:
      break;

    case FDT_PROP:
      res = ufdt_new_node(fdtp, cur_fdt_tag_offset, pool);
      break;

    case FDT_BEGIN_NODE:
      res = ufdt_new_node(fdtp, cur_fdt_tag_offset, pool);

      do {
        cur_fdt_tag_offset = *next_fdt_tag_offset;
        tag = fdt_next_tag(fdtp, cur_fdt_tag_offset, next_fdt_tag_offset);
        child_node = fdt_to_ufdt_tree(fdtp, cur_fdt_tag_offset,
                                      next_fdt_tag_offset, tag, pool);
        ufdt_node_add_child(res, child_node);
      } while (tag != FDT_END_NODE);
      break;

    default:
      break;
  }

  return res;
}

void ufdt_print(struct ufdt *tree) {
  ufdt_node_print(tree->root, 0);
}

struct ufdt_node *ufdt_get_node_by_path_len(struct ufdt *tree, const char *path,
                                            int len) {
  /*
   * RARE: aliases
   * In device tree, we can assign some alias to specific nodes by defining
   * these relation in "/aliases" node.
   * The node has the form:
   * {
   *   a = "/a_for_apple";
   *   b = "/b_for_banana";
   * };
   * So the path "a/subnode_1" should be expanded to "/a_for_apple/subnode_1".
   */
  if (*path != '/') {
    const char *end = path + len;

    const char *next_slash;
    next_slash = dto_memchr(path, '/', end - path);
    if (!next_slash) next_slash = end;

    struct ufdt_node *aliases_node =
        ufdt_node_get_node_by_path(tree->root, "/aliases");
    aliases_node = ufdt_node_get_property_by_name_len(aliases_node, path,
                                                      next_slash - path);

    int path_len = 0;
    const char *alias_path =
        ufdt_node_get_fdt_prop_data(aliases_node, &path_len);

    if (alias_path == NULL) {
      dto_error("Failed to find alias %s\n", path);
      return NULL;
    }

    struct ufdt_node *target_node =
        ufdt_node_get_node_by_path_len(tree->root, alias_path, path_len);

    return ufdt_node_get_node_by_path_len(target_node, next_slash,
                                          end - next_slash);
  }
  return ufdt_node_get_node_by_path_len(tree->root, path, len);
}

struct ufdt_node *ufdt_get_node_by_path(struct ufdt *tree, const char *path) {
  return ufdt_get_node_by_path_len(tree, path, dto_strlen(path));
}

struct ufdt_node *ufdt_get_node_by_phandle(struct ufdt *tree,
                                           uint32_t phandle) {
  struct ufdt_node *res = NULL;
  /*
   * Do binary search in phandle_table.data.
   * [s, e) means the possible range which contains target node.
   */
  int s = 0, e = tree->phandle_table.len;
  while (e - s > 1) {
    int mid = s + ((e - s) >> 1);
    uint32_t mid_phandle = tree->phandle_table.data[mid].phandle;
    if (phandle < mid_phandle)
      e = mid;
    else
      s = mid;
  }
  if (e - s > 0 && tree->phandle_table.data[s].phandle == phandle) {
    res = tree->phandle_table.data[s].node;
  }
  return res;
}

static int count_phandle_node(struct ufdt_node *node) {
  if (node == NULL) return 0;
  if (ufdt_node_tag(node) != FDT_BEGIN_NODE) return 0;
  int res = 0;
  if (ufdt_node_get_phandle(node) > 0) res++;
  struct ufdt_node **it;
  for_each_child(it, node) { res += count_phandle_node(*it); }
  return res;
}

static void set_phandle_table_entry(struct ufdt_node *node,
                                    struct ufdt_phandle_table_entry *data,
                                    int *cur) {
  if (node == NULL || ufdt_node_tag(node) != FDT_BEGIN_NODE) return;
  int ph = ufdt_node_get_phandle(node);
  if (ph > 0) {
    data[*cur].phandle = ph;
    data[*cur].node = node;
    (*cur)++;
  }
  struct ufdt_node **it;
  for_each_node(it, node) set_phandle_table_entry(*it, data, cur);
  return;
}

int phandle_table_entry_cmp(const void *pa, const void *pb) {
  uint32_t ph_a = ((const struct ufdt_phandle_table_entry *)pa)->phandle;
  uint32_t ph_b = ((const struct ufdt_phandle_table_entry *)pb)->phandle;
  if (ph_a < ph_b)
    return -1;
  else if (ph_a == ph_b)
    return 0;
  else
    return 1;
}

struct ufdt_static_phandle_table build_phandle_table(struct ufdt *tree) {
  struct ufdt_static_phandle_table res;
  res.len = count_phandle_node(tree->root);
  res.data = dto_malloc(sizeof(struct ufdt_phandle_table_entry) * res.len);
  int cur = 0;
  set_phandle_table_entry(tree->root, res.data, &cur);
  dto_qsort(res.data, res.len, sizeof(struct ufdt_phandle_table_entry),
            phandle_table_entry_cmp);
  return res;
}

struct ufdt *ufdt_from_fdt(void *fdtp, size_t fdt_size,
                           struct ufdt_node_pool *pool) {
  (void)(fdt_size); /* unused parameter */

  int start_offset = fdt_path_offset(fdtp, "/");
  if (start_offset < 0) {
    return ufdt_construct(NULL, pool);
  }

  struct ufdt *res_tree = ufdt_construct(fdtp, pool);
  int end_offset;
  int start_tag = fdt_next_tag(fdtp, start_offset, &end_offset);
  res_tree->root =
      fdt_to_ufdt_tree(fdtp, start_offset, &end_offset, start_tag, pool);

  res_tree->phandle_table = build_phandle_table(res_tree);

  return res_tree;
}

static int _ufdt_get_property_nameoff(const struct ufdt *tree, const char *name,
                                      const struct ufdt_prop_dict *dict) {
  int res;
  const struct fdt_property *same_name_prop = ufdt_prop_dict_find(dict, name);
  if (same_name_prop != NULL) {
    /* There is a property with same name, just use its string offset */
    res = fdt32_to_cpu(same_name_prop->nameoff);
  } else {
    /* Get the string offset from the string table of the current tree */
    res = ufdt_get_string_off(tree, name);
    if (res == 0) {
      dto_error("Cannot find property name in string table: %s\n", name);
      return 0;
    }
  }
  return res;
}

static int _ufdt_output_property_to_fdt(
    const struct ufdt *tree, void *fdtp,
    const struct ufdt_node_fdt_prop *prop_node, struct ufdt_prop_dict *dict) {
  int nameoff = _ufdt_get_property_nameoff(tree, prop_node->name, dict);
  if (nameoff == 0) return -1;

  int data_len = 0;
  void *data = ufdt_node_get_fdt_prop_data(&prop_node->parent, &data_len);
  int aligned_data_len = (data_len + (FDT_TAGSIZE - 1)) & ~(FDT_TAGSIZE - 1);

  int new_propoff = fdt_size_dt_struct(fdtp);
  int new_prop_size = sizeof(struct fdt_property) + aligned_data_len;
  struct fdt_property *new_prop =
      (struct fdt_property *)((char *)fdtp + fdt_off_dt_struct(fdtp) +
                              new_propoff);
  char *fdt_end = (char *)fdtp + fdt_totalsize(fdtp);
  if ((char *)new_prop + new_prop_size > fdt_end) {
    dto_error("Not enough space for adding property.\n");
    return -1;
  }
  fdt_set_size_dt_struct(fdtp, new_propoff + new_prop_size);

  new_prop->tag = cpu_to_fdt32(FDT_PROP);
  new_prop->nameoff = cpu_to_fdt32(nameoff);
  new_prop->len = cpu_to_fdt32(data_len);
  dto_memcpy(new_prop->data, data, data_len);

  ufdt_prop_dict_add(dict, new_prop);

  return 0;
}

static int _ufdt_output_node_to_fdt(const struct ufdt *tree, void *fdtp,
                                    const struct ufdt_node *node,
                                    struct ufdt_prop_dict *dict) {
  uint32_t tag = ufdt_node_tag(node);

  if (tag == FDT_PROP) {
    return _ufdt_output_property_to_fdt(
        tree, fdtp, (const struct ufdt_node_fdt_prop *)node, dict);
  }

  int err = fdt_begin_node(fdtp, ufdt_node_name(node));
  if (err < 0) return -1;

  struct ufdt_node **it;
  for_each_prop(it, node) {
    err = _ufdt_output_node_to_fdt(tree, fdtp, *it, dict);
    if (err < 0) return -1;
  }

  for_each_node(it, node) {
    err = _ufdt_output_node_to_fdt(tree, fdtp, *it, dict);
    if (err < 0) return -1;
  }

  err = fdt_end_node(fdtp);
  if (err < 0) return -1;

  return 0;
}

static int _ufdt_output_strtab_to_fdt(const struct ufdt *tree, void *fdt) {
  /* Currently, we don't know the final dt_struct size, so we copy all
     string tables to the end of the target fdt buffer in reversed order.
     At last, fdt_finish() will adjust dt_string offset */
  const char *struct_top =
      (char *)fdt + fdt_off_dt_struct(fdt) + fdt_size_dt_struct(fdt);
  char *dest = (char *)fdt + fdt_totalsize(fdt);

  int dest_size = 0;
  for (int i = 0; i < tree->num_used_fdtps; i++) {
    void *src_fdt = tree->fdtps[i];
    const char *src_strtab = (const char *)src_fdt + fdt_off_dt_strings(src_fdt);
    int strtab_size = fdt_size_dt_strings(src_fdt);

    dest -= strtab_size;
    if (dest < struct_top) {
      dto_error("Not enough space for string table.\n");
      return -1;
    }

    dto_memcpy(dest, src_strtab, strtab_size);

    dest_size += strtab_size;
  }

  fdt_set_size_dt_strings(fdt, dest_size);

  return 0;
}

int ufdt_to_fdt(const struct ufdt *tree, void *buf, int buf_size) {
  if (tree->num_used_fdtps == 0) return -1;

  int err;
  err = fdt_create(buf, buf_size);
  if (err < 0) return -1;

  /* Here we output the memory reserve map of the ONLY FIRST fdt,
     to be in compliance with the DTO behavior of libfdt. */
  int n_mem_rsv = fdt_num_mem_rsv(tree->fdtps[0]);
  for (int i = 0; i < n_mem_rsv; i++) {
    uint64_t addr, size;
    fdt_get_mem_rsv(tree->fdtps[0], i, &addr, &size);
    fdt_add_reservemap_entry(buf, addr, size);
  }

  err = fdt_finish_reservemap(buf);
  if (err < 0) return -1;

  err = _ufdt_output_strtab_to_fdt(tree, buf);
  if (err < 0) return -1;

  struct ufdt_prop_dict dict;
  err = ufdt_prop_dict_construct(&dict, buf);
  if (err < 0) return -1;

  err = _ufdt_output_node_to_fdt(tree, buf, tree->root, &dict);
  if (err < 0) return -1;

  ufdt_prop_dict_destruct(&dict);

  err = fdt_finish(buf);
  if (err < 0) return -1;

  /*
   * IMPORTANT: fdt_totalsize(buf) might be less than buf_size
   * so this is needed to make use of remain spaces.
   */
  return fdt_open_into(buf, buf, buf_size);
}
