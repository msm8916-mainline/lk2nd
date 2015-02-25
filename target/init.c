/*
 * Copyright (c) 2008 Travis Geiselbrecht
 *
 * Copyright (c) 2015, The Linux Foundation. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include <err.h>
#include <debug.h>
#include <target.h>
#include <compiler.h>
#include <dload_util.h>
#include <sdhci_msm.h>

#define EXPAND(NAME) #NAME
#define TARGET(NAME) EXPAND(NAME)
/*
 * default implementations of these routines, if the target code
 * chooses not to implement.
 */

__WEAK void target_early_init(void)
{
}

__WEAK void target_init(void)
{
}

__WEAK void *target_get_scratch_address(void)
{
    return (void *)(SCRATCH_ADDR);
}

__WEAK unsigned target_get_max_flash_size(void)
{
    return (120 * 1024 * 1024);
}

__WEAK int target_is_emmc_boot(void)
{
#if _EMMC_BOOT
    return 1;
#else
    return 0;
#endif
}

__WEAK unsigned check_reboot_mode(void)
{
    return 0;
}

__WEAK unsigned check_hard_reboot_mode(void)
{
    return 0;
}

__WEAK void reboot_device(unsigned reboot_reason)
{
}

__WEAK uint32_t is_user_force_reset(void)
{
	return 0;
}

__WEAK int set_download_mode(enum dload_mode mode)
{
	return -1;
}

__WEAK unsigned target_pause_for_battery_charge(void)
{
    return 0;
}

__WEAK unsigned target_baseband()
{
	return 0;
}

__WEAK void target_serialno(unsigned char *buf)
{
	snprintf((char *) buf, 13, "%s",TARGET(BOARD));
}

__WEAK void target_fastboot_init()
{
}

__WEAK int emmc_recovery_init(void)
{
	return 0;
}

__WEAK bool target_use_signed_kernel(void)
{
#if _SIGNED_KERNEL
	return 1;
#else
	return 0;
#endif
}

__WEAK bool target_is_ssd_enabled(void)
{
#ifdef SSD_ENABLE
	return 1;
#else
	return 0;
#endif
}

__WEAK void target_load_ssd_keystore(void)
{
}

/* Default target does not support continuous splash screen feature. */
__WEAK int target_cont_splash_screen()
{
	return 0;
}

/* Default target specific initialization before using USB */
__WEAK void target_usb_init(void)
{
}

/* Default target specific usb shutdown */
__WEAK void target_usb_stop(void)
{
}

/* Default target specific target uninit */
__WEAK void target_uninit(void)
{
}

__WEAK bool target_display_panel_node(char *panel_name, char *pbuf,
	uint16_t buf_size)
{
	return false;
}

__WEAK void target_display_init(const char *panel_name)
{
}

__WEAK void target_display_shutdown(void)
{
}

__WEAK uint8_t target_panel_auto_detect_enabled()
{
	return 0;
}

__WEAK uint8_t target_is_edp()
{
	return 0;
}

/* default usb controller to be used. */
__WEAK const char * target_usb_controller()
{
	return "ci";
}

/* override for target specific usb phy reset. */
__WEAK void target_usb_phy_reset(void)
{
}

/* determine if target is in warm boot. */
__WEAK bool target_warm_boot(void)
{
	return false;
}

/* Determine the HLOS subtype of the target */
__WEAK uint32_t target_get_hlos_subtype(void)
{
	return 0;
}

/* Initialize crypto parameters */
__WEAK void target_crypto_init_params()
{
}

/* Default CFG delay value */
__WEAK uint32_t target_ddr_cfg_val()
{
	return DDR_CONFIG_VAL;
}

/* Return Build variant */
__WEAK bool target_build_variant_user()
{
#if USER_BUILD_VARIANT
	return true;
#else
	return false;
#endif
}
