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

#include "ufdt_prop_dict.h"

#include "libfdt.h"

#include "libufdt_sysdeps.h"

#define UFDT_PROP_DICT_INIT_SZ 4

/* Empirical values for hash functions. */
#define HASH_BASE 13131

#define DICT_LIMIT_NUM 2
#define DICT_LIMIT_DEN 3

static int _ufdt_prop_dict_str_hash(const char *str) {
  int res = 0;

  for (; *str != '\0'; str++) {
    res *= HASH_BASE;
    res += *str;
  }

  return res;
}

static const struct fdt_property **_ufdt_prop_dict_find_index_by_name(
    const struct ufdt_prop_dict *dict, const char *name) {
  /* size should be 2^k for some k */
  int hash = _ufdt_prop_dict_str_hash(name);
  int size = dict->mem_size;
  int idx = hash & (size - 1);
  /* If collision, use linear probing to find idx in the hash table */
  for (int i = 0; i < size; i++) {
    const struct fdt_property **prop_ptr = &dict->props[idx];
    const struct fdt_property *prop = *prop_ptr;
    if (prop == NULL) return prop_ptr;

    const char *prop_name = fdt_string(dict->fdtp, fdt32_to_cpu(prop->nameoff));
    if (dto_strcmp(prop_name, name) == 0) return prop_ptr;

    idx = (idx + 1) & (size - 1);
  }
  return NULL;
}

static const struct fdt_property **_ufdt_prop_dict_find_index(
    struct ufdt_prop_dict *dict, const struct fdt_property *prop) {
  const char *name = fdt_string(dict->fdtp, fdt32_to_cpu(prop->nameoff));

  return _ufdt_prop_dict_find_index_by_name(dict, name);
}

int _ufdt_prop_dict_construct_int(struct ufdt_prop_dict *dict, void *fdtp,
                                  int size) {
  const size_t entry_size = sizeof(const struct fdt_property *);
  const struct fdt_property **props =
      (const struct fdt_property **)dto_malloc(size * entry_size);
  if (props == NULL) return -1;
  dto_memset(props, 0, size * entry_size);

  dict->mem_size = size;
  dict->num_used = 0;
  dict->fdtp = fdtp;
  dict->props = props;

  return 0;
}

int ufdt_prop_dict_construct(struct ufdt_prop_dict *dict, void *fdtp) {
  return _ufdt_prop_dict_construct_int(dict, fdtp, UFDT_PROP_DICT_INIT_SZ);
}

void ufdt_prop_dict_destruct(struct ufdt_prop_dict *dict) {
  if (dict == NULL) return;

  dto_free(dict->props);
}

static int _ufdt_prop_dict_enlarge_if_needed(struct ufdt_prop_dict *dict) {
  if (dict == NULL) return -1;

  /* Enlarge if the used number over than DICT_LIMIT_NUM / DICT_LIMIT_DEN */
  if (dict->num_used * DICT_LIMIT_DEN <= dict->mem_size * DICT_LIMIT_NUM) {
    return 0;
  }

  int new_size = dict->mem_size * 2;
  struct ufdt_prop_dict temp_dict;
  _ufdt_prop_dict_construct_int(&temp_dict, dict->fdtp, new_size);

  for (int i = 0; i < dict->mem_size; i++) {
    const struct fdt_property *prop = dict->props[i];
    if (prop == NULL) continue;
    const struct fdt_property **new_prop_ptr =
        _ufdt_prop_dict_find_index(&temp_dict, prop);
    if (new_prop_ptr == NULL) {
      dto_error("ufdt_prop_dict: failed to find new index when enlarging.\n");
      ufdt_prop_dict_destruct(&temp_dict);
      return -1;
    }
    *new_prop_ptr = prop;
  }

  dto_free(dict->props);

  dict->mem_size = new_size;
  dict->props = temp_dict.props;

  return 0;
}

int ufdt_prop_dict_add(struct ufdt_prop_dict *dict,
                       const struct fdt_property *prop) {
  const struct fdt_property **prop_ptr = _ufdt_prop_dict_find_index(dict, prop);
  if (prop_ptr == NULL) {
    dto_error("ufdt_prop_dict: failed to find new index when adding.\n");
    return -1;
  }

  if (*prop_ptr == NULL) dict->num_used++;
  *prop_ptr = prop;

  return _ufdt_prop_dict_enlarge_if_needed(dict);
}

const struct fdt_property *ufdt_prop_dict_find(const struct ufdt_prop_dict *dict,
                                               const char *name) {
  const struct fdt_property **prop_ptr =
      _ufdt_prop_dict_find_index_by_name(dict, name);
  return prop_ptr ? *prop_ptr : NULL;
}
