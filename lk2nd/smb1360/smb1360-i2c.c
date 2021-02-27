#include <debug.h>
#include <err.h>
#include <libfdt.h>
#include <platform/timer.h>
#include "smb1360-i2c.h"
#include "../regmap/regmap-gpio-i2c.h"

/* FG registers (on different I2C address) */
#define FG_CFG_I2C_ADDR			0x1
#define FG_PROFILE_A_ADDR		0x2
#define FG_PROFILE_B_ADDR		0x3

#define SMB1360_I2C_ADDR		0x14
#define SMB1360_FG_I2C_ADDR		(SMB1360_I2C_ADDR | FG_CFG_I2C_ADDR)

static gpio_i2c_info_t gpio_i2c = {
	/* .sda/.scl are set dynamically based on DTB */
	GPIO_I2C_CLOCKS
};

static const struct regmap_gpio_i2c regmap = {
	.map = REGMAP_GPIO_I2C("smb1360"),
	.addr = SMB1360_I2C_ADDR,
};

static const struct regmap_gpio_i2c fg_regmap = {
	.map = REGMAP_GPIO_I2C("smb1360-fg"),
	.addr = SMB1360_FG_I2C_ADDR,
};

static const struct smb1360 smb1360 = {
	.regmap = &regmap.map,
	.fg_regmap = &fg_regmap.map,
};

const struct smb1360 *smb1360_setup_i2c(const void *fdt, int offset)
{
	if (!gpio_i2c_read_pins(fdt, offset, &gpio_i2c))
		return NULL;

	gpio_i2c_add_bus(0, &gpio_i2c);
	return &smb1360;
}

status_t smb1360_enable_fg_access(const struct smb1360 *smb)
{
	status_t ret;
	uint8_t val;
	int try;

	ret = regmap_set_bits(smb->regmap, CMD_I2C_REG, FG_ACCESS_ENABLED_BIT);
	if (ret)
		return ret;

	for (try = 0; try <= 50; ++try) {
		ret = regmap_read(smb->regmap, IRQ_I_REG, &val);
		if (ret)
			goto err;
		if (val & IRQ_I_FG_ACCESS_ALLOWED_BIT)
			return 0;
		mdelay(100);
	}

	ret = ERR_TIMED_OUT;
	dprintf(CRITICAL, "smb1360_enable_fg_access timed out\n");

err:
	smb1360_disable_fg_access(smb);
	return ret;
}

void smb1360_disable_fg_access(const struct smb1360 *smb)
{
	regmap_clear_bits(smb->regmap, CMD_I2C_REG, FG_ACCESS_ENABLED_BIT);
}
