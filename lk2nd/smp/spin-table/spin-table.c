// SPDX-License-Identifier: BSD-3-Clause
/* Copyright (c) 2021-2022, Stephan Gerhold <stephan@gerhold.net> */

#include <boot.h>
#include <debug.h>
#include <libfdt.h>

#include <lk2nd/util/cmdline.h>
#include <lk2nd/util/lkfdt.h>
#include <lk2nd/util/mmu.h>
#include <lk2nd/util/psci.h>

#include "../cpu-boot.h"

#define SMP_SPIN_TABLE_MAGIC	BE64(0x7370696e2d746162)  /* "spin-tab" */

struct smp_spin_table {
	uint8_t code[128] __ALIGNED(128);
	uint64_t magic;
	uint64_t release_addr;
};

static uint8_t spin_code_a64[] = {
	0x5f, 0x20, 0x03, 0xd5,	/* wfe */
	0x3e, 0x04, 0x00, 0x58,	/* ldr	lr, 0x88 */
	0xde, 0xff, 0xff, 0xb4,	/* cbz	lr, 0 */
	0xc0, 0x03, 0x1f, 0xd6,	/* br	lr */
};
static uint8_t spin_code_a32[] = {
	0x02, 0xf0, 0x20, 0xe3,	/* wfe */
	0x7c, 0xe0, 0x9f, 0xe5,	/* ldr	lr, [pc, #124]; 0x88 */
	0x00, 0x00, 0x5e, 0xe3,	/* cmp	lr, #0 */
	0xfb, 0xff, 0xff, 0x0a,	/* beq	0 */
	0x1e, 0xff, 0x2f, 0xe1,	/* bx	lr */
};

static int check_cpus(const void *dtb, bool arm64, bool force)
{
	int cpus, node;

	if (is_scm_armv8_support()) {
		int32_t psciv = psci_version();
		if (psciv != PSCI_RET_NOT_SUPPORTED) {
			dprintf(INFO, "PSCI v%d.%d detected, skipping SMP spin table\n",
				PSCI_VERSION_MAJOR(psciv), PSCI_VERSION_MINOR(psciv));
			if (!force)
				return -FDT_ERR_BADOVERLAY;

			dprintf(CRITICAL,
				"WARNING: Using spin-table when PSCI is supported bypasses "
				"the TZ firmware and might cause strange issues!\n");
		}
	} else if (arm64) {
		dprintf(CRITICAL, "Cannot boot ARM64 with old SCM calling convention\n");
		return -FDT_ERR_INTERNAL;
	}

	cpus = fdt_path_offset(dtb, "/cpus");
	if (cpus < 0) {
		dprintf(CRITICAL, "Cannot find /cpus node: %d\n", cpus);
		return cpus;
	}

	/*
	 * Look for any CPU node and see if "psci" (or "spin-table" directly)
	 * is requested as "enable-method". At this point we already know that
	 * PSCI is unsupported.
	 *
	 * NOTE: This assumes that all CPUs have the same enable-method...
	 */
	node = fdt_subnode_offset(dtb, cpus, "cpu");
	if (node < 0) {
		dprintf(CRITICAL, "Cannot find any CPU node: %d\n", node);
		return node;
	}

	if (!force && lkfdt_prop_strneq(dtb, node, "enable-method", "psci") &&
	    lkfdt_prop_strneq(dtb, node, "enable-method", "spin-table")) {
		dprintf(INFO, "Custom CPU enable-method detected, skipping SMP spin table\n");
		return -FDT_ERR_BADOVERLAY;
	}

	return cpus;
}

static struct smp_spin_table *map_reserved_table(const void *dtb)
{
	int parent, node, ret;
	uint32_t addr, size;
	const char *name;

	parent = fdt_path_offset(dtb, "/reserved-memory");
	if (parent < 0) {
		dprintf(CRITICAL, "Cannot find /reserved-memory node for SMP spin table: %d\n", parent);
		return NULL;
	}

	/*
	 * Look for dedicated spin-table@... reserved-memory, but fall back to
	 * rmtfs@ if it is missing. Since CPUs should typically be brought up
	 * during early initialization, it is sufficient if the reserved-memory
	 * belongs to something else that is writable by the CPU and is only
	 * needed later in the boot process. RMTFS is only needed for the modem,
	 * which should be booted long after the CPU cores have been brought up.
	 */
	node = fdt_subnode_offset(dtb, parent, "spin-table");
	if (node == -FDT_ERR_NOTFOUND)
		node = fdt_subnode_offset(dtb, parent, "rmtfs");
	if (node < 0) {
		dprintf(CRITICAL, "No suitable reserved memory for SMP spin table found: %d\n", node);
		return NULL;
	}

	name = fdt_get_name(dtb, node, NULL);
	ret = lkfdt_get_reg(dtb, parent, node, &addr, &size);
	if (ret < 0) {
		dprintf(CRITICAL, "Failed to read reserved-memory reg of %s: %d\n", name, ret);
		return NULL;
	}
	dprintf(INFO, "Using reserved memory %s (%#08x size %#x) for SMP spin table\n", name, addr, size);
	if (size < sizeof(struct smp_spin_table)) {
		dprintf(CRITICAL, "Reserved memory %s is too small for SMP spin table (%u < %u)\n",
			name, size, sizeof(struct smp_spin_table));
		return NULL;
	}
	if (addr % __alignof__(struct smp_spin_table)) {
		dprintf(CRITICAL, "Reserved memory %s is not aligned correctly for SMP spin table\n", name);
		return NULL;
	}

	if (!lk2nd_mmu_map_ram_wt("SMP spin table", addr, sizeof(struct smp_spin_table)))
		return NULL;

	return (struct smp_spin_table *)addr;
}

static void boot_and_setup_cpu(void *dtb, int cpu, int cpus, struct smp_spin_table *smp)
{
	int ret, node;
	uint32_t mpidr;

	ret = lkfdt_get_reg(dtb, cpus, cpu, &mpidr, NULL);
	if (ret < 0) {
		dprintf(CRITICAL, "Failed to read CPU node reg: %d\n", ret);
		return;
	}

	/* Adjust device tree with properties needed for spin-table */
	ret = fdt_setprop_u64(dtb, cpu, "cpu-release-addr",
			      (uintptr_t)&smp->release_addr);
	if (ret) {
		dprintf(CRITICAL, "Failed to set cpu-release-addr: %d\n", ret);
		return;
	}
	ret = fdt_setprop_string(dtb, cpu, "enable-method", "spin-table");
	if (ret) {
		dprintf(CRITICAL, "Failed to update enable-method: %d\n", ret);
		return;
	}

	if (!cpu_boot(dtb, cpu, mpidr))
		return;

	/* Enable the SAW/SPM node for CPU idle functionality */
	node = lkfdt_lookup_phandle(dtb, cpu, "qcom,saw");
	if (node < 0) {
		dprintf(CRITICAL, "Cannot find qcom,saw node: %d\n", node);
		return;
	}

	if (!lkfdt_node_is_available(dtb, node)) {
		ret = fdt_nop_property(dtb, node, "status");
		if (ret)
			dprintf(CRITICAL, "Failed to enable SAW/SPM node: %d\n", ret);
	}
}

static void setup_idle_states(void *dtb, int node)
{
	int ret, state_node;

	/* Keep device tree as-is if entry-method is not "psci" */
	if (lkfdt_prop_strneq(dtb, node, "entry-method", "psci"))
		return;

	ret = fdt_nop_property(dtb, node, "entry-method");
	if (ret)
		dprintf(CRITICAL, "Failed to remove idle-states entry-method: %d\n", ret);

	fdt_for_each_subnode(state_node, dtb, node) {
		if (lkfdt_prop_strneq(dtb, state_node, "idle-state-name",
				      "standalone-power-collapse") == 0) {
			ret = fdt_setprop_string(dtb, state_node, "compatible",
						 "qcom,idle-state-spc");
			if (ret)
				dprintf(CRITICAL, "Failed to set qcom,idle-state-spc compatible: %d\n", ret);
		}
	}
	if (state_node < 0 && state_node != -FDT_ERR_NOTFOUND) {
		dprintf(CRITICAL, "Failed to read idle-states subnodes: %d\n", state_node);
		return;
	}
}

static int lk2nd_smp_spin_table_setup(void *dtb, const char *cmdline,
				      enum boot_type boot_type)
{
	struct smp_spin_table *smp;
	int cpus, ret, node;

	/* Downstream boots CPUs itself */
	if (boot_type & (BOOT_DOWNSTREAM | BOOT_LK2ND))
		return 0;

	cpus = check_cpus(dtb, boot_type & BOOT_ARM64,
			  lk2nd_cmdline_scan(cmdline, "lk2nd.spin-table=force"));
	if (cpus < 0)
		return 0; /* Still continue boot without SMP spin table */

	smp = map_reserved_table(dtb);
	if (!smp)
		return 0; /* Still continue boot without SMP spin table */

	/* Prepare spin table memory */
	memset(smp, 0, sizeof(*smp));
	if (boot_type & BOOT_ARM64)
		memcpy(smp->code, spin_code_a64, sizeof(spin_code_a64));
	else
		memcpy(smp->code, spin_code_a32, sizeof(spin_code_a32));

	ret = cpu_boot_set_addr((uintptr_t)smp->code, boot_type & BOOT_ARM64);
	if (ret) {
		dprintf(CRITICAL, "Failed to set CPU boot address: %d\n", ret);
		return 0; /* Still continue boot without SMP spin table */
	}

	/* Disable the /psci node if needed */
	node = fdt_path_offset(dtb, "/psci");
	if (node >= 0 && lkfdt_node_is_available(dtb, node)) {
		ret = fdt_setprop_string(dtb, node, "status", "disabled");
		if (ret) {
			dprintf(CRITICAL, "Failed to disable /psci node: %d\n", ret);
			return ret;
		}
	}

	smp->magic = SMP_SPIN_TABLE_MAGIC;
	fdt_for_each_subnode(node, dtb, cpus) {
		const char *name = fdt_get_name(dtb, node, &ret);
		if (!name)
			return ret;
		if (strncmp(name, "cpu@", strlen("cpu@")) == 0)
			boot_and_setup_cpu(dtb, node, cpus, smp);
		if (strcmp(name, "idle-states") == 0)
			setup_idle_states(dtb, node);
	}
	if (node < 0 && node != -FDT_ERR_NOTFOUND) {
		dprintf(CRITICAL, "Failed to read /cpus subnodes: %d\n", node);
		return node;
	}
	return 0;
}
DEV_TREE_UPDATE(lk2nd_smp_spin_table_setup);
