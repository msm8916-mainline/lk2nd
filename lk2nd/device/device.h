/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef LK2ND_DEVICE_DEVICE_H
#define LK2ND_DEVICE_DEVICE_H

struct lk2nd_panel {
	const char *name;
};

struct lk2nd_device {
	const char *compatible;
	const char *model;

	struct lk2nd_panel panel;

#if WITH_LK2ND_DEVICE_2ND
	const char *cmdline;
	const char *bootloader;
	const char *serialno;
	const char *device;
	const char *carrier;
	const char *radio;
#endif
};
extern struct lk2nd_device lk2nd_dev;

#if WITH_LK2ND_DEVICE_2ND
const void *lk2nd_device2nd_init(void);
int lk2nd_device2nd_match_device_node(const void *dtb, int lk2nd_node);
#else
static inline const void *lk2nd_device2nd_init(void) { return NULL; }
static inline int lk2nd_device2nd_match_device_node(const void *dtb, int lk2nd_node) { return -1; }
#endif

#endif /* LK2ND_DEVICE_DEVICE_H */
