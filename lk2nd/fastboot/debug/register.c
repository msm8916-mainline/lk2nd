// SPDX-License-Identifier: BSD-3-Clause
/* Copyright (c) 2022-2023, Stephan Gerhold <stephan@gerhold.net> */

#include <fastboot.h>
#include <reg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define FASTBOOT_REGISTER_OEM_READ(name, uintn_t, fmt) \
static void cmd_oem_read##name(const char *arg, void *data, unsigned sz) \
{ \
	char response[MAX_RSP_SIZE]; \
	uintptr_t addr = atoul(arg); \
	if (addr % __alignof__(uintn_t)) { \
		fastboot_fail("unaligned address"); \
		return; \
	} \
	snprintf(response, sizeof(response), fmt "\n", read##name(addr)); \
	fastboot_info(response); \
	fastboot_okay(""); \
} \
FASTBOOT_REGISTER("oem debug read" #name, cmd_oem_read##name)

static bool parse_write_args(const char *arg, uintptr_t *addr, uint64_t *val)
{
	char *saveptr;
	char *args = strdup(arg);
	char *addr_str = strtok_r(args, " ", &saveptr);
	char *value_str = strtok_r(NULL, " ", &saveptr);

	if (!addr_str || !value_str) {
		free(args);
		return false;
	}

	*addr = atoul(addr_str);
	*val = atoull(value_str);
	free(args);
	return true;
}

#define FASTBOOT_REGISTER_OEM_WRITE(name, uintn_t) \
static void cmd_oem_write##name(const char *arg, void *data, unsigned sz) \
{ \
	uintptr_t addr; \
	uint64_t val; \
	if (!parse_write_args(arg, &addr, &val)) { \
		fastboot_fail("missing address/value"); \
		return; \
	} \
	if (addr % __alignof__(uintn_t)) { \
		fastboot_fail("unaligned address"); \
		return; \
	} \
	write##name(val, addr); \
	fastboot_okay(""); \
} \
FASTBOOT_REGISTER("oem debug write" #name, cmd_oem_write##name)

FASTBOOT_REGISTER_OEM_READ(b, uint8_t, "%#02x");
FASTBOOT_REGISTER_OEM_WRITE(b, uint8_t);
FASTBOOT_REGISTER_OEM_READ(hw, uint16_t, "%#04x");
FASTBOOT_REGISTER_OEM_WRITE(hw, uint16_t);
FASTBOOT_REGISTER_OEM_READ(l, uint32_t, "%#08x");
FASTBOOT_REGISTER_OEM_WRITE(l, uint32_t);
FASTBOOT_REGISTER_OEM_READ(q, uint64_t, "%#016llx");
FASTBOOT_REGISTER_OEM_WRITE(q, uint64_t);

#if WITH_DEV_PMIC_PM8X41
#include "pm8x41_hw.h"
#define readpmic(addr)		REG_READ(addr)
#define writepmic(val, addr)	REG_WRITE(addr, val)
FASTBOOT_REGISTER_OEM_READ(pmic, uint8_t, "%#02x");
FASTBOOT_REGISTER_OEM_WRITE(pmic, uint8_t);
#endif

#if WITH_DEV_PMIC_PM8921
#include "dev/pm8921.h"

static uint8_t readpmic(unsigned short addr)
{
	uint8_t val = 0;
	if (pm8921_dev)
		pm8921_dev->read(&val, 1, addr);
	return val;
}

static void writepmic(uint8_t val, unsigned short addr)
{
	if (pm8921_dev)
		pm8921_dev->write(&val, 1, addr);
}

FASTBOOT_REGISTER_OEM_READ(pmic, uint8_t, "%#02x");
FASTBOOT_REGISTER_OEM_WRITE(pmic, uint8_t);
#endif
