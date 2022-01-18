/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __LK2ND_SMP_CPU_BOOT_H
#define __LK2ND_SMP_CPU_BOOT_H

int qcom_set_boot_addr(uint32_t addr, bool arm64);
void qcom_power_up_arm_cortex(uint32_t mpidr, uint32_t base);
void qcom_power_up_kpssv2(uint32_t mpidr, uint32_t base, uint32_t l2_saw_base);

#endif
