#ifndef UFDT_NODE_POOL_H
#define UFDT_NODE_POOL_H

/*
 * Copyright (C) 2017 The Android Open Source Project
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

struct ufdt_node_pool_block_header;

struct ufdt_node_pool {
  /* A single linked list contains all blocks.
     Blocks with one or more unused entries are in front;
     blocks without unused entry are in rear. */
  struct ufdt_node_pool_block_header *first_block;
  struct ufdt_node_pool_block_header **last_block_ptr;
};

void ufdt_node_pool_construct(struct ufdt_node_pool *pool);
void ufdt_node_pool_destruct(struct ufdt_node_pool *pool);

void *ufdt_node_pool_alloc(struct ufdt_node_pool *pool);
void ufdt_node_pool_free(struct ufdt_node_pool *pool, void *node);

#endif
