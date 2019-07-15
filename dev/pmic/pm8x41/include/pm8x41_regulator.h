#ifndef _PM8x41_REGULATOR_H_
#define _PM8x41_REGULATOR_H_

struct spmi_regulator {
	const char *name;
	uint32_t base;
	const struct spmi_regulator_mapping *mapping;
};

bool regulator_is_enabled(struct spmi_regulator *vreg);
int regulator_get_voltage(struct spmi_regulator *vreg);

void regulators_init(struct spmi_regulator *vregs);
struct spmi_regulator* target_get_regulators();

#endif
