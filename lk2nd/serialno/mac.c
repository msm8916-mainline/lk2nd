// SPDX-License-Identifier: BSD-3-Clause
/* Copyright (c) 2022, Stephan Gerhold <stephan@gerhold.net> */

#include <boot.h>
#include <fastboot.h>
#include <stdlib.h>
#include <string.h>
#include <target.h>

#include <libfdt.h>

#define MAC_ADDR_SIZE		6

/* 2 hexadecimal chars per byte plus separators and null terminator */
#define MAC_ADDR_STR_SIZE	(MAC_ADDR_SIZE * 3)

static const char * const mac_compatibles[] = {
	"qcom,wcnss-wlan",
};
static const char * const bd_compatibles[] = {
	"qcom,wcnss-bt",
};

static uint8_t mac_addr[MAC_ADDR_SIZE];
static uint8_t bd_addr[MAC_ADDR_SIZE];

static bool generate_addrs(void)
{
	char sn_buf[13] = "0";
	char *start, *end;
	unsigned long sn;

	if (mac_addr[0])
		return true;

	/* Take the last 8 chars (4 bytes hexadecimal) from the serial number */
	target_serialno((unsigned char *)sn_buf);
	start = MAX(sn_buf, sn_buf + strlen(sn_buf) - 8);
	sn = strtoul(start, &end, 16);
	if (sn == 0) {
		dprintf(CRITICAL, "Cannot generate MAC address from serial number %s\n",
			start);
		return false;
	}

	/* locally administrated pool */
	mac_addr[0] = 0x02;
	mac_addr[1] = 0x00;
	mac_addr[2] = sn >> 24;
	mac_addr[3] = sn >> 16;
	mac_addr[4] = sn >> 8;
	mac_addr[5] = sn;

	/*
	 * BD address is encoded in little endian format (reversed),
	 * with least significant bit flipped.
	 */
	bd_addr[5] = 0x02;
	bd_addr[4] = 0x00;
	bd_addr[3] = sn >> 24;
	bd_addr[2] = sn >> 16;
	bd_addr[1] = sn >> 8;
	bd_addr[0] = sn ^ 1;

	dprintf(SPEW, "Generated MAC address %02X:%02X:%02X:%02X:%02X:%02X, "
		"BD address %02X:%02X:%02X:%02X:%02X:%02X\n",
		mac_addr[0], mac_addr[1], mac_addr[2],
		mac_addr[3], mac_addr[4], mac_addr[5],
		bd_addr[5], bd_addr[4], bd_addr[3],
		bd_addr[2], bd_addr[1], bd_addr[0]);
	return true;
}

static bool check_existing_prop(void *dtb, int node, const char *prop)
{
	const uint8_t *val;
	int len, i;

	val = fdt_getprop(dtb, node, prop, &len);
	if (!val)
		return false;

	if (len != MAC_ADDR_SIZE) {
		dprintf(CRITICAL, "Invalid %s length: %d\n", prop, len);
		return false;
	}

	/* Check if the prop is all zero as placeholder */
	for (i = 0; i < MAC_ADDR_SIZE; ++i)
		if (val[i])
			return true;

	return false;
}

static int update_dt(void *dtb, const char * const compatibles[], unsigned num,
		     const char *prop, uint8_t mac[MAC_ADDR_SIZE])
{
	unsigned i;

	for (i = 0; i < num; ++i) {
		int node, ret;

		node = fdt_node_offset_by_compatible(dtb, -1, compatibles[i]);
		if (node == -FDT_ERR_NOTFOUND)
			continue;

		if (check_existing_prop(dtb, node, prop)) {
			dprintf(INFO, "%s already has a %s\n", compatibles[i], prop);
			continue;
		}

		if (!generate_addrs())
			return 0; /* Still continue boot */

		ret = fdt_setprop(dtb, node, prop, mac, MAC_ADDR_SIZE);
		if (ret < 0)
			return ret;
	}

	return 0;
}

static int lk2nd_mac_dt_update(void *dtb, const char *cmdline,
			       enum boot_type boot_type)
{
	int ret;

	/* Downstream has different ways to set the MAC address */
	if (boot_type & (BOOT_DOWNSTREAM | BOOT_LK2ND))
		return 0;

	ret = update_dt(dtb, mac_compatibles, ARRAY_SIZE(mac_compatibles),
			"local-mac-address", mac_addr);
	if (ret)
		return ret;

	ret = update_dt(dtb, bd_compatibles, ARRAY_SIZE(bd_compatibles),
			"local-bd-address", bd_addr);
	if (ret)
		return ret;

	return 0;
}
DEV_TREE_UPDATE(lk2nd_mac_dt_update);

static void lk2nd_mac_fastboot_register(void) {
	char *wifimac, *btmac;

	if (!generate_addrs())
		return;

	wifimac = malloc(2 * MAC_ADDR_STR_SIZE);
	if (!wifimac)
		return;

	btmac = &wifimac[MAC_ADDR_STR_SIZE];
	sprintf(wifimac, "%02X:%02X:%02X:%02X:%02X:%02X",
		mac_addr[0], mac_addr[1], mac_addr[2],
		mac_addr[3], mac_addr[4], mac_addr[5]);
	sprintf(btmac, "%02X:%02X:%02X:%02X:%02X:%02X",
		bd_addr[5], bd_addr[4], bd_addr[3],
		bd_addr[2], bd_addr[1], bd_addr[0]);

	fastboot_publish("btmacaddr", btmac);
	fastboot_publish("wifimacaddr", wifimac);
}
FASTBOOT_INIT(lk2nd_mac_fastboot_register);
