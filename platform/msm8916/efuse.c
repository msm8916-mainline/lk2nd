#include <reg.h>
#include <platform/efuse.h>

#define EFUSE1	0x0005c004
#define EFUSE	0x0005c00c

void efuse_read_speed_bin(int *bin, int *version) {
	uint32_t pte_efuse = readl(EFUSE1);

	*version = (pte_efuse >> 18) & 0x3;
	if (!*version) {
		*bin = (pte_efuse >> 23) & 0x3;
		if (*bin)
			return;
	}

	pte_efuse = readl(EFUSE);
	*bin = (pte_efuse >> 2) & 0x7;
}
