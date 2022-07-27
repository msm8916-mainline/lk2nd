/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef GPIO_SUPPLIER_H
#define GPIO_SUPPLIER_H

#include <bits.h>
#include <stdbool.h>
#include <stdint.h>

/* Common helpers */
#define GPIOL_CONF_DRVSTR(x)		BITS_SHIFT(x, 11, 8)

#define PMIC_GPIO_NUM_PIN(num)		BITS_SHIFT(num, 7, 0)
#define PMIC_GPIO_NUM_SID(num)		BITS_SHIFT(num, 15, 8)

#define PMIC_FLAGS_VIN_SEL(x)		BITS_SHIFT(num, 26, 24)
#define PMIC_NON_DEFAULT_VIN_SEL	BIT(27)

#endif /* GPIO_SUPPLIER_H */
