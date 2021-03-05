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

status_t smb1360_check_cycle_stretch(const struct smb1360 *smb)
{
	uint8_t val;
	status_t ret = regmap_read(smb->regmap, STATUS_4_REG, &val);

	if (ret)
		return ret;

	if (!(val & CYCLE_STRETCH_ACTIVE_BIT))
		return 0;

	return regmap_set_bits(smb->regmap, CMD_I2C_REG, CYCLE_STRETCH_CLEAR_BIT);
}

status_t smb1360_reload(const struct smb1360 *smb)
{
	return regmap_set_bits(smb->regmap, CMD_I2C_REG, RELOAD_BIT);
}

/*
 * FIXME: This resets some FG registers but not all of them properly.
 * All in all I'm not sure if this has really positive effects only.
 */

#define OTP_BACKUP_START		0xE0
#define OTP_BACKUP_MAP_REG		0xF0

static status_t smb1360_otp_backup_reset(const struct smb1360 *smb)
{
	uint8_t backup_empty[16] = {0};
	uint16_t map_empty = 0;
	status_t ret;

	/* Clear OTP backup map */
	ret = regmap_raw_write(smb->fg_regmap, OTP_BACKUP_MAP_REG,
			       &map_empty, sizeof(map_empty));
	if (ret)
		return ret;

	/* Clear OTP backup */
	ret = regmap_raw_write(smb->fg_regmap, OTP_BACKUP_START,
			       backup_empty, sizeof(backup_empty));
	if (ret)
		return ret;

	return 0;
}

status_t smb1360_fg_reset(const struct smb1360 *smb)
{

	status_t ret;

	ret = smb1360_enable_fg_access(smb);
	if (ret)
		return ret;

	ret = smb1360_otp_backup_reset(smb);
	if (ret)
		goto out;

	/* delay for the FG access to settle */
	mdelay(1500);

	ret = regmap_set_bits(smb->regmap, CMD_I2C_REG, FG_RESET_BIT);
	if (ret)
		goto out;

	/* delay for FG reset */
	mdelay(1500);

	ret = regmap_clear_bits(smb->regmap, CMD_I2C_REG, FG_RESET_BIT);
	if (ret)
		goto out;

out:
	smb1360_disable_fg_access(smb);
	smb1360_check_cycle_stretch(smb);
	return ret;
}
