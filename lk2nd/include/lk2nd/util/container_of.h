/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef LK2ND_CONTAINER_OF_H
#define LK2ND_CONTAINER_OF_H

#define container_of(ptr, type, member) \
	((type *)((addr_t)(ptr) - offsetof(type, member)))

#endif
