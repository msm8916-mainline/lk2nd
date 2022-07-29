/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef LK2ND_UTIL_MMU_H
#define LK2ND_UTIL_MMU_H

/**
 * lk2nd_mmu_map_ram() - Validate and map RAM with one of the specified flags.
 * @name: Name of the memory region (for debugging purposes)
 * @start: Start address of the memory region
 * @size: Total size of the memory region
 * @flags1: First MMU mapping flags to try
 * @flags2: Second MMU mapping flags to try (as a fallback)
 *
 * Check that @start is really part of the RAM, then try to map it using @flags1.
 * In case of conflicts with existing memory mappings, try falling back to @flags2.
 *
 * Return: true if mapping was successful, false otherwise
 */
bool lk2nd_mmu_map_ram(const char *name, uintptr_t start, uint32_t size,
		       uint flags1, uint flags2);

/**
 * lk2nd_mmu_map_ram() - Validate and map RAM with write-through cache.
 * @name: Name of the memory region (for debugging purposes)
 * @start: Start address of the memory region
 * @size: Total size of the memory region
 *
 * Check that @start is really part of the RAM, then try to map it using
 * write-through caching.
 *
 * Return: true if mapping was successful, false otherwise
 */
bool lk2nd_mmu_map_ram_wt(const char *name, uintptr_t start, uint32_t size);

/**
 * lk2nd_mmu_map_ram() - Validate and map RAM based on existing mappings
 * @name: Name of the memory region (for debugging purposes)
 * @start: Start address of the memory region
 * @size: Total size of the memory region
 *
 * Check that @start is really part of the RAM, then try to map it using
 * write-through caching. If that fails (usually because there are existing
 * mappings using other flags) try to map it using write-back-allocate caching.
 * This is mainly a workaround for the inconsistent memory mappings used on
 * various platforms. The caller is responsible to handle both mapping types
 * correctly, usually by flushing the cache where necessary.
 *
 * Return: true if mapping was successful, false otherwise
 */
bool lk2nd_mmu_map_ram_dynamic(const char *name, uintptr_t start, uint32_t size);

#endif /* LK2ND_UTIL_MMU_H */
