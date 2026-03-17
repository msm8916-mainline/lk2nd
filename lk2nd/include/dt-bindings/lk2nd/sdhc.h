/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef DT_BINDINGS_LK2ND_SDHC_H
#define DT_BINDINGS_LK2ND_SDHC_H

/*
 * Macro to set a phandle like 'SDC\x00'.
 * Implementation will extract the slot number from this.
 */
#define SDHC_DEV_MAGIC 0x53444300
#define SDHC_DEV_PHANDLE(slot) (SDHC_DEV_MAGIC | ((slot) & 0xf))

#define SDHC_SLOT_2 2
#define SDHC_SLOT_3 3

#endif /* DT_BINDINGS_LK2ND_SDHC_H */
