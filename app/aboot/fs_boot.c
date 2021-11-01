// SPDX-License-Identifier: GPL-2.0-only

#include <debug.h>
#include <target.h>
#include <string.h>

#include <lib/bio.h>
#include <lib/fs.h>

#include "fs_boot.h"

struct fs_boot_data fs_boot_data;

static const char *bootable_parts[] = {
	"system",
	"cache",
	"userdata",
};

static bool fsboot_bootable_part(char *label)
{
	int i;
	for (i = 0; i < ARRAY_SIZE(bootable_parts); ++i)
		if (strcmp(label, bootable_parts[i]) == 0)
			return true;

	return false;
}

static enum rproc_mode fsboot_load_rproc_mode(const char *path)
{
	char mode[9] = {0};
	char *lf;
	int ret;

	ret = fs_load_file(path, mode, sizeof(mode) - 1);
	if (ret < 0) {
		dprintf(INFO, "Failed to load rproc mode: %d\n", ret);
		return RPROC_MODE_UNKNOWN;
	}

	/* Trim new line from string */
	lf = strchr(mode, '\n');
	if (lf)
		*lf = 0;

	if (strcmp(mode, "all") == 0)
		return RPROC_MODE_ALL;
	if (strcmp(mode, "no-modem") == 0)
		return RPROC_MODE_NO_MODEM;
	if (strcmp(mode, "none") == 0)
		return RPROC_MODE_NONE;

	dprintf(CRITICAL, "Unknown rproc mode: %s\n", mode);
	return RPROC_MODE_UNKNOWN;
}

/* Only iterate through files if target is NULL */
static int fsboot_fs_load_img(char *dev_name, void* target, size_t sz)
{
	struct dirhandle *dirh;
	struct dirent dirent;

	char image_path[128] = "/mnt/";
	int ret = -1;
	bool path_valid = false;

	if (fs_mount("/mnt", "ext2", dev_name) < 0)
		return -1;

	ret = fs_open_dir("/mnt", &dirh);
	if (ret < 0) {
		dprintf(SPEW, "fs_open_dir ret = %d\n", ret);
		goto out;
	}

	while (fs_read_dir(dirh, &dirent) >= 0) {
		if (!target)
			dprintf(SPEW, "| /%s/%s\n", dev_name, dirent.name);
		if (!path_valid && strncmp(dirent.name, "boot.img", 7) == 0) {
			strcpy(&image_path[strlen("/mnt/")], dirent.name);
			dprintf(INFO, "Found boot image: %s : %s\n", dev_name, image_path);
			path_valid = true;
		} else if (strncmp(dirent.name, "lk2nd_skip", 10) == 0) {
			dprintf(INFO, "Partition skipped: %s\n", dev_name);
			fs_close_dir(dirh);
			goto out;
		}
	}
	fs_close_dir(dirh);

	if (target && path_valid)
		ret = fs_load_file(image_path, target, sz);

	if (ret >= 0 && path_valid && !fs_boot_data.dev) {
		fs_boot_data.rproc_mode = fsboot_load_rproc_mode("/mnt/lk2nd_rproc_mode");
		dprintf(INFO, "Boot partition rproc mode: %d\n", fs_boot_data.rproc_mode);
	}

out:
	fs_unmount("/mnt");
	return ret;
}

static int fsboot_find_and_boot(int bdev_id, void* target, size_t sz)
{
	int i = 0, j = 0, ret = 0;
	char dev_name[128];
	bdev_t *dev = NULL;

	/* HACK: There is no hd1p0 for some reason */
	if (bdev_id == 1)
		i = 1;

	sprintf(dev_name, "hd%d", bdev_id);
	dev = bio_open(dev_name);
	if (!dev) {
		dprintf(CRITICAL, "fs-boot: Can't open %s\n", dev_name);
		return -1;
	}
	bio_close(dev);

	if (!target)
		dprintf(SPEW, "fs-boot: Looking at %s:\n", dev_name);

	sprintf(dev_name, "hd%dp%d", bdev_id, i);
	while (dev = bio_open(dev_name)) {
		/* Only probe useful partitions if looking at emmc */
		if (bdev_id == FS_BOOT_DEV_EMMC && !fsboot_bootable_part(dev->label)) {
			bio_close(dev);
			i++;
			sprintf(dev_name, "hd%dp%d", bdev_id, i);
			continue;
		}
		if (!target)
			dprintf(SPEW, "%.8s:  %.10s (%6llu MiB): \n",dev->name, dev->label, dev->size / (1024 * 1024));
		bio_close(dev);

		ret = fsboot_fs_load_img(dev_name, target, sz);
		if (ret >= 0)
			fs_boot_data.dev = bdev_id;
		if (target && ret >= 0)
			return ret;

		/* Only check subpartitions on emmc */
		if (ret < 0 && bdev_id == FS_BOOT_DEV_EMMC) {
			j = 0;
			sprintf(dev_name, "hd%dp%dp%d", bdev_id, i, j);
			while (dev = bio_open(dev_name)) {
				if (!target)
					dprintf(SPEW, "%.8s:  %.10s (%6llu MiB): \n",dev->name, dev->label, dev->size / (1024 * 1024));
				bio_close(dev);
				ret = fsboot_fs_load_img(dev_name, target, sz);
				if (ret >= 0)
					fs_boot_data.dev = bdev_id;
				if (target && ret >= 0)
					return ret;

				j++;
				sprintf(dev_name, "hd%dp%dp%d", bdev_id, i, j);
			}
		}
		i++;
		sprintf(dev_name, "hd%dp%d", bdev_id, i);
	}

	return -1;
}

void fsboot_test(void)
{
	dprintf(SPEW, "fs-boot: Scanned devices:\n");
	fsboot_find_and_boot(FS_BOOT_DEV_SDCARD, NULL, 0);
	fsboot_find_and_boot(FS_BOOT_DEV_EMMC, NULL, 0);
}

int fsboot_boot_first(void* target, size_t sz)
{
	int ret = -1;

	ret = fsboot_find_and_boot(FS_BOOT_DEV_SDCARD, target, sz);
	if (ret > 0)
		return ret;

	ret = fsboot_find_and_boot(FS_BOOT_DEV_EMMC, target, sz);
	if (ret > 0)
		return ret;

	return -1;
}
