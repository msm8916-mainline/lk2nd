// SPDX-License-Identifier: BSD-3-Clause
/* Copyright (c) 2022-2023, Stephan Gerhold <stephan@gerhold.net> */

#include <arch/defines.h>
#include <fastboot.h>
#include <printf.h>
#include <stdint.h>

#define read_cpuid(val, opc1, crm, opc2) \
	__asm__("mrc p15, " #opc1 ", %0, c0, " #crm ", " #opc2 : "=r"(val))
#define write_cpuid(val, opc1, crm, opc2) \
	__asm__("mcr p15, " #opc1 ", %0, c0, " #crm ", " #opc2 :: "r"(val))

static void dump_cache_info(unsigned int lvl, bool ind, const char *ctype)
{
	char response[MAX_RSP_SIZE];
	uint32_t csselr, ccsidr;

	csselr = lvl << 1 | ind << 0;
	write_cpuid(csselr, 2, c0, 0);
	isb();
	read_cpuid(ccsidr, 1, c0, 0);

	snprintf(response, sizeof(response), "L%u %s cache: CCSIDR = %#x",
		 lvl + 1, ctype, ccsidr);
	fastboot_info(response);
}

static void dump_all_cache_info(void)
{
	uint32_t clidr, ctype;
	unsigned int lvl;

	read_cpuid(clidr, 1, c0, 1);
	for (lvl = 0; lvl < 7; ++lvl, clidr >>= 3) {
		ctype = clidr & 0b111;
		if (!ctype)
			break;
		if (ctype & 0b001)
			dump_cache_info(lvl, true, "instruction");
		if (ctype & 0b010)
			dump_cache_info(lvl, false, "data");
		if (ctype == 0b100)
			dump_cache_info(lvl, false, "unified");
	}
}

#define fastboot_info_cpuid(name, opc1, crm, opc2) do { \
	char response[MAX_RSP_SIZE]; \
	uint32_t val; \
	read_cpuid(val, opc1, crm, opc2); \
	snprintf(response, sizeof(response), #name " = %#x", val); \
	fastboot_info(response); \
} while(0)

static void cmd_oem_debug_cpuid(const char *arg, void *data, unsigned sz)
{
	fastboot_info_cpuid(MIDR, 0, c0, 0);
	fastboot_info_cpuid(CTR, 0, c0, 1);
	fastboot_info_cpuid(TCMTR, 0, c0, 2);
	fastboot_info_cpuid(TLBTR, 0, c0, 3);
	fastboot_info_cpuid(MPIDR, 0, c0, 5);
	fastboot_info_cpuid(REVIDR, 0, c0, 6);
	fastboot_info_cpuid(ID_PFR0, 0, c1, 0);
	fastboot_info_cpuid(ID_PFR1, 0, c1, 1);
	fastboot_info_cpuid(ID_PFR2, 0, c3, 4);
	fastboot_info_cpuid(ID_DFR0, 0, c1, 2);
	fastboot_info_cpuid(ID_DFR1, 0, c3, 5);
	fastboot_info_cpuid(ID_AFR0, 0, c1, 3);
	fastboot_info_cpuid(ID_MMFR0, 0, c1, 4);
	fastboot_info_cpuid(ID_MMFR1, 0, c1, 5);
	fastboot_info_cpuid(ID_MMFR2, 0, c1, 6);
	fastboot_info_cpuid(ID_MMFR3, 0, c1, 7);
	fastboot_info_cpuid(ID_MMFR4, 0, c2, 6);
	fastboot_info_cpuid(ID_MMFR5, 0, c3, 6);
	fastboot_info_cpuid(ID_ISAR0, 0, c2, 0);
	fastboot_info_cpuid(ID_ISAR1, 0, c2, 1);
	fastboot_info_cpuid(ID_ISAR2, 0, c2, 2);
	fastboot_info_cpuid(ID_ISAR3, 0, c2, 3);
	fastboot_info_cpuid(ID_ISAR4, 0, c2, 4);
	fastboot_info_cpuid(ID_ISAR5, 0, c2, 5);
	fastboot_info_cpuid(ID_ISAR6, 0, c2, 7);

	fastboot_info_cpuid(CLIDR, 1, c0, 1);
	dump_all_cache_info();

	fastboot_okay("");
}
FASTBOOT_REGISTER("oem debug cpuid", cmd_oem_debug_cpuid);
