/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __LK2ND_SMP_CPU_BOOT_H
#define __LK2ND_SMP_CPU_BOOT_H

int qcom_set_boot_addr(uint32_t addr);
void qcom_power_up_arm_cortex(uint32_t mpidr, uint32_t base);

#endif
