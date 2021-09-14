/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef LK2ND_SMP_CPU_BOOT_H
#define LK2ND_SMP_CPU_BOOT_H

int cpu_boot_set_addr(uintptr_t addr, bool arm64);

bool cpu_boot(const void *dtb, int node, uint32_t mpidr);
void cpu_boot_cortex_a(uint32_t base, uint32_t apcs_base);
void cpu_boot_kpssv1(uint32_t reg, uint32_t saw_reg);
void cpu_boot_kpssv2(uint32_t reg, uint32_t l2_saw_base);

#endif /* LK2ND_SMP_CPU_BOOT_H */
