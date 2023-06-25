// SPDX-License-Identifier: GPL-2.0-only
#include <debug.h>
#include <libfdt.h>
#include <lk2nd.h>
#include <psci.h>
#include <scm.h>

#include "cpu-boot.h"

struct smp_spin_table {
	uint8_t code[4096];
	uint64_t release_addr;
};

static uint8_t smp_spin_table_a64[] = {
	0x5f, 0x20, 0x03, 0xd5,	/* wfe */
	0xfe, 0x7f, 0x00, 0x58,	/* ldr	lr, 0x1000 */
	0xde, 0xff, 0xff, 0xb4,	/* cbz	lr, 0 */
	0xc0, 0x03, 0x1f, 0xd6,	/* br	lr */
};
static uint8_t smp_spin_table_a32[] = {
	0x02, 0xf0, 0x20, 0xe3,	/* wfe */
	0xf4, 0xef, 0x9f, 0xe5,	/* ldr	lr, [pc, #4084] */
	0x00, 0x00, 0x5e, 0xe3,	/* cmp	lr, #0 */
	0xfb, 0xff, 0xff, 0x0a,	/* beq	0 */
	0x1e, 0xff, 0x2f, 0xe1,	/* bx	lr */
};

static int lkfdt_lookup_phandle(void *fdt, int node, const char *prop_name)
{
	const uint32_t *phandle;
	int len;

	phandle = fdt_getprop(fdt, node, prop_name, &len);
	if (len < 0)
		return len;
	if (len != sizeof(uint32_t))
		return -FDT_ERR_BADVALUE;

	return fdt_node_offset_by_phandle(fdt, fdt32_to_cpu(*phandle));
}

static void smp_spin_table_setup_cpu(struct smp_spin_table *table,
				     void *fdt, int cpu_node)
{
	const uint32_t *val;
	int node, len, ret;
	uint32_t cpu, base;

	val = fdt_getprop(fdt, cpu_node, "reg", &len);
	if (len != sizeof(*val)) {
		dprintf(CRITICAL, "Cannot read reg property of CPU node: %d\n", len);
		return;
	}
	cpu = fdt32_to_cpu(*val);
	dprintf(INFO, "Booting CPU%x\n", cpu);

	/* Adjust device tree with properties needed for spin-table */
	ret = fdt_setprop_u64(fdt, cpu_node, "cpu-release-addr",
			      (uintptr_t)&table->release_addr);
	if (ret) {
		dprintf(CRITICAL, "Failed to set cpu-release-addr: %d\n", ret);
		return;
	}

	ret = fdt_setprop_string(fdt, cpu_node, "enable-method", "spin-table");
	if (ret) {
		dprintf(CRITICAL, "Failed to update enable-method: %d\n", ret);
		return;
	}

	/* Power up the CPU core using registers in the ACC node */
	node = lkfdt_lookup_phandle(fdt, cpu_node, "qcom,acc");
	if (node < 0) {
		dprintf(CRITICAL, "Cannot find qcom,acc node: %d\n", node);
		return;
	}

	val = fdt_getprop(fdt, node, "reg", &len);
	if (len < sizeof(*val)) {
		dprintf(CRITICAL, "Cannot read reg property of qcom,acc node: %d\n", len);
		return;
	}
	base = fdt32_to_cpu(*val);

#if TARGET_MSM8916 || TARGET_MSM8226 || TARGET_MSM8909
	qcom_power_up_arm_cortex(cpu, base);
#elif TARGET_MSM8974
	node = lkfdt_lookup_phandle(fdt, cpu_node, "next-level-cache");
	if (node < 0) {
		dprintf(CRITICAL, "Cannot find next-level-cache: %d\n", node);
		return;
	}

	node = lkfdt_lookup_phandle(fdt, node, "qcom,saw");
	if (node < 0) {
		dprintf(CRITICAL, "Cannot find L2 SAW node: %d\n", node);
		return;
	}

	val = fdt_getprop(fdt, node, "reg", &len);
	if (len < sizeof(*val)) {
		dprintf(CRITICAL, "Cannot read reg property of L2 qcom,saw node: %d\n", len);
		return;
	}
	qcom_power_up_kpssv2(cpu, base, fdt32_to_cpu(*val));
#else
#error Unsupported target for CPU spin-table!
#endif

	/* Enable the SAW/SPM node for CPU idle functionality */
	node = lkfdt_lookup_phandle(fdt, cpu_node, "qcom,saw");
	if (node < 0) {
		dprintf(CRITICAL, "Cannot find qcom,saw node: %d\n", node);
		return;
	}

	if (!lkfdt_node_is_available(fdt, node)) {
		ret = fdt_setprop_string(fdt, node, "status", "okay");
		if (ret)
			dprintf(CRITICAL, "Failed to enable SAW/SPM node: %d\n", ret);
	}
}

static void smp_spin_table_setup_idle_states(void *fdt, int node)
{
	int ret, state_node;

	/* Keep device tree as-is if entry-method is not "psci" */
	if (lkfdt_prop_strcmp(fdt, node, "entry-method", "psci"))
		return;

	ret = fdt_nop_property(fdt, node, "entry-method");
	if (ret)
		dprintf(CRITICAL, "Failed to NOP idle-states entry-method: %d\n", ret);

	fdt_for_each_subnode(state_node, fdt, node) {
		const char *name;
		int len;

		name = fdt_getprop(fdt, state_node, "idle-state-name", &len);
		if (len < 0 || name[len]) {
			dprintf(CRITICAL, "Failed to get idle-state-name: %d\n", len);
			continue;
		}

		if (strcmp(name, "standalone-power-collapse") == 0) {
			ret = fdt_setprop_string(fdt, state_node, "compatible", "qcom,idle-state-spc");
			if (ret)
				dprintf(CRITICAL, "Failed to set qcom,idle-state-spc compatible: %d\n", ret);
		}

	}
	if (state_node < 0 && state_node != -FDT_ERR_NOTFOUND) {
		dprintf(CRITICAL, "Failed to read idle-states subnodes: %d\n", state_node);
		return;
	}
}

/*
 * If the other CPU cores are booted in aarch64 state before the main CPU
 * switches to aarch64, qhypstub has no way to detect that and will boot them
 * in EL1 instead of EL2 (assuming it was bypassed for the state switch).
 * To avoid that, force execution state to aarch64.
 */
extern void qhypstub_set_state_aarch64(void);

void smp_spin_table_setup(struct smp_spin_table *table, void *fdt,
			  bool arm64, bool force)
{
	int offset, node, ret;

	if (is_scm_armv8_support()) {
		scmcall_arg arg = {PSCI_0_2_FN_PSCI_VERSION};
		uint32_t psci_version = scm_call2(&arg, NULL);

		if (psci_version != PSCI_RET_NOT_SUPPORTED) {
			dprintf(INFO, "PSCI v%d.%d detected, no need for SMP spin table\n",
				PSCI_VERSION_MAJOR(psci_version), PSCI_VERSION_MINOR(psci_version));
			if (!force)
				return;

			dprintf(CRITICAL,
				"WARNING: Using spin-table when PSCI is supported bypasses "
				"the TZ firmware and might cause strange issues!\n");
		}
	} else if (arm64) {
		dprintf(CRITICAL, "Cannot boot ARM64 with 32-bit TZ :(\n");
		return;
	}

	offset = fdt_path_offset(fdt, "/psci");
	if (offset >= 0 && lkfdt_node_is_available(fdt, offset)) {
		ret = fdt_setprop_string(fdt, offset, "status", "disabled");
		if (ret)
			dprintf(CRITICAL, "Failed to set psci to status = \"disabled\": %d\n", ret);
	}

	offset = fdt_path_offset(fdt, "/cpus");
	if (offset < 0) {
		dprintf(CRITICAL, "Cannot find /cpus node: %d\n", offset);
		return;
	}

	/*
	 * Look for any CPU node and see if "psci" (or "spin-table" directly)
	 * is requested as "enable-method". At this point we already know that
	 * PSCI is unsupported so we replace it with "spin-table" if necessary.
	 *
	 * NOTE: This assumes that all CPUs have the same enable-method!
	 */
	node = fdt_subnode_offset(fdt, offset, "cpu");
	if (node < 0) {
		dprintf(CRITICAL, "Cannot find any CPU node: %d\n", ret);
		return;
	}

	if (!force && lkfdt_prop_strcmp(fdt, node, "enable-method", "psci") &&
	    lkfdt_prop_strcmp(fdt, node, "enable-method", "spin-table")) {
		dprintf(INFO, "Custom CPU enable-method detected, no need for SMP spin table\n");
		return;
	}

	if (arm64)
		memcpy(table->code, smp_spin_table_a64, sizeof(smp_spin_table_a64));
	else
		memcpy(table->code, smp_spin_table_a32, sizeof(smp_spin_table_a32));

	table->release_addr = 0;

	ret = qcom_set_boot_addr((uint32_t)table, arm64);
	if (ret) {
		dprintf(CRITICAL, "Failed to set CPU boot address: %d\n", ret);
		return;
	}

#if TARGET_MSM8916
	if (arm64)
		qhypstub_set_state_aarch64();
#endif

	fdt_for_each_subnode(node, fdt, offset) {
		const char *name;
		int len;

		name = fdt_get_name(fdt, node, &len);
		if (len < strlen("cpu@") || name[len])
			continue;
		if (strncmp(name, "cpu@", strlen("cpu@")) == 0)
			smp_spin_table_setup_cpu(table, fdt, node);
		if (strcmp(name, "idle-states") == 0)
			smp_spin_table_setup_idle_states(fdt, node);
	}
	if (node < 0 && node != -FDT_ERR_NOTFOUND) {
		dprintf(CRITICAL, "Failed to read /cpus subnodes: %d\n", node);
		return;
	}
}
