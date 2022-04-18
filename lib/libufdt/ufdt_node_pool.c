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

#include "ufdt_node_pool.h"

#include "libufdt_sysdeps.h"
#include "ufdt_types.h"

/* Define DEBUG_DISABLE_POOL to use dto_malloc and dto_free directly */
/* #define DEBUG_DISABLE_POOL */

#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define UFDT_NODE_POOL_ENTRIES_PER_BLOCK 1024
/* UFDT_NODE_POOL_ENTRY_SIZE must be equal to or larger than
   sizeof(struct ufdt_node_fdt_node) and sizeof(struct ufdt_node_fdt_prop) */
#define UFDT_NODE_POOL_ENTRY_SIZE \
  MAX(sizeof(struct ufdt_node_fdt_node), sizeof(struct ufdt_node_fdt_prop))
/* A block is a header appended UFDT_NODE_POOL_ENTRIES_PER_BLOCK entries */
#define UFDT_NODE_POOL_BLOCK_SIZE               \
  (sizeof(struct ufdt_node_pool_block_header) + \
   UFDT_NODE_POOL_ENTRY_SIZE * UFDT_NODE_POOL_ENTRIES_PER_BLOCK)

/*
 * The data structure:
 *
 *        pool                   block                     block
 *  +--------------+     +--------------------+     +-----------------+
 *  | *first_block |---->| block_header:      |     | ...             | ...
 *  | ...          |     |  *next_block       |---->|                 |---->
 *  +--------------+     |  *first_free_entry |--\  | ...             |
 *                       |  ...               |  |
 *                       +--------------------+  |
 *                       | entry_header:      |<-/
 *                       |  *next             |--\
 *                       +--------------------+  |
 *                       |  ...               |<-/
 *                       |                    |--\
 *                       +--------------------+  |
 *                       |  ...               |  v
 */

struct ufdt_node_pool_entry_header {
  struct ufdt_node_pool_entry_header *next;
};

struct ufdt_node_pool_block_header {
  struct ufdt_node_pool_block_header *next_block;
  struct ufdt_node_pool_entry_header *first_free_entry;
  int alloc_entry_cnt;
};

void ufdt_node_pool_construct(struct ufdt_node_pool *pool) {
  pool->first_block = NULL;
  pool->last_block_ptr = &pool->first_block;
}

void ufdt_node_pool_destruct(struct ufdt_node_pool *pool) {
  int is_leak = 0;
  struct ufdt_node_pool_block_header *block = pool->first_block;
  while (block != NULL) {
    if (block->alloc_entry_cnt != 0) is_leak = 1;

    struct ufdt_node_pool_block_header *next_block = block->next_block;
    dto_free(block);
    block = next_block;
  }

  if (is_leak) {
    dto_error("Some nodes aren't freed before ufdt_node_pool_destruct().\n");
  }

  pool->first_block = NULL;
  pool->last_block_ptr = NULL;
}

static struct ufdt_node_pool_block_header *_ufdt_node_pool_create_block() {
  char *block_buf = (char *)dto_malloc(UFDT_NODE_POOL_BLOCK_SIZE);
  char *block_buf_start = block_buf + sizeof(struct ufdt_node_pool_block_header);
  char *block_buf_end = block_buf + UFDT_NODE_POOL_BLOCK_SIZE;

  struct ufdt_node_pool_block_header *block =
      (struct ufdt_node_pool_block_header *)block_buf;

  // Setup all entries to be a one way link list
  struct ufdt_node_pool_entry_header **next_ptr = &block->first_free_entry;
  for (char *entry_buf = block_buf_start; entry_buf < block_buf_end;
       entry_buf += UFDT_NODE_POOL_ENTRY_SIZE) {
    struct ufdt_node_pool_entry_header *entry =
        (struct ufdt_node_pool_entry_header *)entry_buf;

    *next_ptr = entry;

    next_ptr = &entry->next;
  }
  *next_ptr = NULL;

  block->next_block = NULL;
  block->alloc_entry_cnt = 0;

  return block;
}

static void _ufdt_node_pool_destory_block(
    struct ufdt_node_pool_block_header *block) {
  dto_free(block);
}

static void *_ufdt_node_pool_block_alloc(
    struct ufdt_node_pool_block_header *block) {
  // take the first free enrtry
  struct ufdt_node_pool_entry_header *entry = block->first_free_entry;

  block->first_free_entry = entry->next;
  block->alloc_entry_cnt++;

  return entry;
}

static void _ufdt_node_pool_block_free(struct ufdt_node_pool_block_header *block,
                                       void *node) {
  // put the node to the first free enrtry
  struct ufdt_node_pool_entry_header *entry = node;
  entry->next = block->first_free_entry;

  block->first_free_entry = entry;
  block->alloc_entry_cnt--;
}

static void _ufdt_node_pool_preppend_block(
    struct ufdt_node_pool *pool, struct ufdt_node_pool_block_header *block) {
  struct ufdt_node_pool_block_header *origin_first_block = pool->first_block;
  block->next_block = origin_first_block;

  pool->first_block = block;
  if (origin_first_block == NULL) {
    pool->last_block_ptr = &block->next_block;
  }
}

static void _ufdt_node_pool_append_block(
    struct ufdt_node_pool *pool, struct ufdt_node_pool_block_header *block) {
  block->next_block = NULL;

  *pool->last_block_ptr = block;
  pool->last_block_ptr = &block->next_block;
}

static void _ufdt_node_pool_remove_block(
    struct ufdt_node_pool *pool,
    struct ufdt_node_pool_block_header **block_ptr) {
  struct ufdt_node_pool_block_header *block = *block_ptr;
  struct ufdt_node_pool_block_header *next_block = block->next_block;

  *block_ptr = next_block;
  if (next_block == NULL) {
    pool->last_block_ptr = block_ptr;
  }

  block->next_block = NULL;
}

void *ufdt_node_pool_alloc(struct ufdt_node_pool *pool) {
#ifdef DEBUG_DISABLE_POOL
  return dto_malloc(UFDT_NODE_POOL_ENTRY_SIZE);
#endif

  // return dto_malloc(UFDT_NODE_POOL_ENTRY_SIZE);
  // If there is no free block, create a new one
  struct ufdt_node_pool_block_header *block = pool->first_block;
  if (block == NULL || block->first_free_entry == NULL) {
    block = _ufdt_node_pool_create_block();
    _ufdt_node_pool_preppend_block(pool, block);
  }

  void *node = _ufdt_node_pool_block_alloc(block);

  // Move the block to the last if there is no free entry
  if (block->first_free_entry == NULL && *pool->last_block_ptr != block) {
    _ufdt_node_pool_remove_block(pool, &pool->first_block);
    _ufdt_node_pool_append_block(pool, block);
  }

  return node;
}

static struct ufdt_node_pool_block_header **_ufdt_node_pool_search_block(
    struct ufdt_node_pool *pool, void *node) {
  struct ufdt_node_pool_block_header **block_ptr = &pool->first_block;
  while (*block_ptr != NULL) {
    struct ufdt_node_pool_block_header *block = *block_ptr;
    const char *block_buf_start =
        (char *)block + sizeof(struct ufdt_node_pool_block_header);
    const char *block_buf_end = (char *)block + UFDT_NODE_POOL_BLOCK_SIZE;

    if ((char *)node >= block_buf_start && (char *)node < block_buf_end) {
      break;
    }

    block_ptr = &block->next_block;
  }
  return block_ptr;
}

void ufdt_node_pool_free(struct ufdt_node_pool *pool, void *node) {
#ifdef DEBUG_DISABLE_POOL
  return dto_free(node);
#endif

  struct ufdt_node_pool_block_header **block_ptr =
      _ufdt_node_pool_search_block(pool, node);
  if (*block_ptr == NULL) {
    dto_error("Given node is not in the pool.\n");
    return;
  }

  struct ufdt_node_pool_block_header *block = *block_ptr;
  _ufdt_node_pool_block_free(block, node);
  _ufdt_node_pool_remove_block(pool, block_ptr);

  /* Delay free block: free the block only if the block is all freed and
      there has at least one another free block */
  if (block->alloc_entry_cnt == 0 && pool->first_block != NULL &&
      pool->first_block->first_free_entry != NULL) {
    _ufdt_node_pool_destory_block(block);
    return;
  }

  _ufdt_node_pool_preppend_block(pool, block);
}
