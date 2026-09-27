/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef DT_BINDINGS_LK2ND_GPIO_H
#define DT_BINDINGS_LK2ND_GPIO_H

/*
 * Macro to set a phandle like 'GIO\x00'.
 * Implementation will derive driver ID from this
 */
#define GPIO_DEV_PHANDLE(dev) (0x47494f00 | ((dev) & 0xff))

#define GPIOL_DEVICE_INVALID	0
#define GPIOL_DEVICE_TLMM	1
#define GPIOL_DEVICE_PMIC	2
#define GPIOL_DEVICE_PMIC_PON	3

/* 0:7 - GPIO configuration flags */
#ifndef BIT
#define BIT(x) (1 << (x))
#endif

#define GPIO_ACTIVE_HIGH	0
#define GPIO_ACTIVE_LOW		BIT(0)

#define GPIO_PUSH_PULL		0
#define GPIO_SINGLE_ENDED	BIT(1)

#define GPIO_LINE_OPEN_SOURCE	0
#define GPIO_LINE_OPEN_DRAIN	BIT(2)

#define GPIO_OPEN_DRAIN		(GPIO_SINGLE_ENDED | GPIO_LINE_OPEN_DRAIN)
#define GPIO_OPEN_SOURCE	(GPIO_SINGLE_ENDED | GPIO_LINE_OPEN_SOURCE)

#define GPIO_PERSISTENT		0
#define GPIO_TRANSITORY		BIT(3)

#define GPIO_BIAS_DISABLE	0
#define GPIO_PULL_UP		BIT(4)
#define GPIO_PULL_DOWN		BIT(5)

/* 8:15 - lk2nd extensions to the GPIO flags */
#define GPIO_DRIVE_STR(x)	(((x) & 0x0f) << 8) /* bits 8:11 */

/* 16:23 - Reserved for internal use (initial state) */

/* 24:31 - Device-specific config flags */

#define GPIO_PMIC_VIN_SEL(x)	(((x) & 0x07) << 24 | BIT(27)) /* bits 24:27 */

/* device-specific definitions */

#define GPIO_PMIC_NUM(num, sid) (((num) & 0xff) | ((sid) & 0xff) << 8)

#define GPIO_PMIC_PWRKEY	1
#define GPIO_PMIC_RESIN		2

#endif /* DT_BINDINGS_LK2ND_GPIO_H */
