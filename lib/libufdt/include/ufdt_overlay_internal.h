/*
 * Copyright (C) 2018 The Android Open Source Project
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

#ifndef UFDT_OVERLAY_INTERNAL_H
#define UFDT_OVERLAY_INTERNAL_H

#include <ufdt_types.h>

void *ufdt_get_fixup_location(struct ufdt *tree, const char *fixup);
int ufdt_do_one_fixup(struct ufdt *tree, const char *fixups, int fixups_len,
                      int phandle);
int ufdt_overlay_do_fixups(struct ufdt *main_tree, struct ufdt *overlay_tree);

enum overlay_result {
  OVERLAY_RESULT_OK,
  OVERLAY_RESULT_MISSING_TARGET,
  OVERLAY_RESULT_MISSING_OVERLAY,
  OVERLAY_RESULT_TARGET_PATH_INVALID,
  OVERLAY_RESULT_TARGET_INVALID,
  OVERLAY_RESULT_MERGE_FAIL,
  OVERLAY_RESULT_VERIFY_FAIL,
};

enum overlay_result ufdt_overlay_get_target(struct ufdt *tree,
                                            struct ufdt_node *frag_node,
                                            struct ufdt_node **target_node);
void ufdt_try_increase_phandle(struct ufdt *tree, uint32_t offset);
uint32_t ufdt_get_max_phandle(struct ufdt *tree);
struct ufdt_static_phandle_table build_phandle_table(struct ufdt *tree);
int ufdt_overlay_do_local_fixups(struct ufdt *tree, uint32_t phandle_offset);
#endif /* UFDT_OVERLAY_INTERNAL_H */
