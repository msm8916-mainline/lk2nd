// SPDX-License-Identifier: BSD-3-Clause
/* Copyright (c) 2023 Nikita Travkin <nikita@trvn.ru> */

#include <debug.h>
#include <decompress.h>
#include <lib/fs.h>
#include <libfdt.h>
#include <platform.h>
#include <platform/iomap.h>
#include <smem.h>
#include <stdlib.h>
#include <target.h>

#include "../../app/aboot/bootimg.h"

#include <lk2nd/boot.h>
#include <lk2nd/device.h>

#include "boot.h"

struct label {
	const char *kernel;
	const char *initramfs;
	const char *dtb;
	const char *dtbdir;
	const char **dtboverlays;
	const char *cmdline;
};

enum token {
	CMD_KERNEL,
	CMD_APPEND,
	CMD_INITRD,
	CMD_FDT,
	CMD_FDTDIR,
	CMD_FDTOVERLAY,
	CMD_UNKNOWN,
};

static const struct {
	char *command;
	enum token token;
} token_map[] = {
	{"kernel",		CMD_KERNEL},
	{"linux",		CMD_KERNEL},
	{"fdtdir",		CMD_FDTDIR},
	{"devicetreedir",	CMD_FDTDIR},
	{"fdt",			CMD_FDT},
	{"devicetree",		CMD_FDT},
	{"fdtoverlays",		CMD_FDTOVERLAY},
	{"devicetree-overlay",	CMD_FDTOVERLAY},
	{"initrd",		CMD_INITRD},
	{"append",		CMD_APPEND},
};

static enum token cmd_to_tok(char *command)
{
	size_t i;

	for (i = 0; i < ARRAY_SIZE(token_map); i++)
		if (!strcmp(command, token_map[i].command))
			return token_map[i].token;

	return CMD_UNKNOWN;
}

#define EOF -1

/**
 * parse_char() - Get one char from the file.
 * @data:    File contents
 * @size:    remaining size of data
 *
 * Update pointers and return the next character.
 *
 * Returns: char value or EOF.
 */
static int parse_char(char **data, size_t *size)
{
	char c = **data;

	if (*size == 0)
		return EOF;

	(*size)--;
	(*data)++;

	return c;
}

/**
 * parse_line() - Read one command from the file.
 * @data:    File contents
 * @size:    remaining size of data
 * @command: returns pointer to the command string
 * @value:   returns pointer to the value string
 *
 * Function scans one line from the data blob, ignoring comments and
 * whitespace; returns pointers to the start of the command and it's
 * value after replacing whitespace and newline after them with \0.
 * @data and @size will be updated to remove the parsed line(s).
 *
 * Returns: 0 on success, negative value on error or EOF.
 */
static int parse_command(char **data, size_t *size, char **command, char **value)
{
	int c;

	/* Step 1: Ignore leading comments and whitespace. */

	while (*size != 0 && (**data == '#' || **data == ' ' || **data == '\t' || **data == '\n')) {
		/* Skip leading whitespace. */
		while (*size != 0 && (**data == ' ' || **data == '\t' || **data == '\n')) {
			c = parse_char(data, size);
			if (c == EOF)
				return -1;
		}

		if (*size != 0 && **data == '#') {
			do {
				c = parse_char(data, size);
				if (c == EOF)
					return -1;
			} while (c != '\n');
		}
	}

	if (*size == 0)
		return -1;

	/* Step 2: Read the command. */
	*command = *data;
	while (*size != 0 && **data != ' ' && **data != '\t' && **data != '\n') {
		c = parse_char(data, size);
		if (c == EOF)
			return -1;
	}

	if (*size != 0 && (**data == ' ' || **data == '\t')) {
		**data = '\0';
		(*data)++;
		(*size)--;
	}

	if (*size == 0 || **data == '\n')
		return -1;

	/* Step 3: Read the value. */

	/* Skip whitespace. */
	while (*size != 0 && (**data == ' ' || **data == '\t' || **data == '\n')) {
		c = parse_char(data, size);
		if (c == EOF || c == '\n')
			return -1;
	}

	*value = *data;
	while (*size != 0 && **data != '\n')
		parse_char(data, size);

	/* The last command may not have a newline. */
	if (*size == 0 || **data == '\n') {
		**data = '\0';
		(*data)++;
		(*size)--;
	}

	return 0;
}

/**
 * parse_conf() - Extract default label from extlinux.conf
 * @data: File contents
 * @size: Length of the file
 * @label: structure to write strings to
 *
 * Find the default label in the file and extract strings from it.
 * This function may destroy the file by changing some newlines to nulls
 * as it may be implemented by pointing into the data buffer to return
 * the configuration strings.
 *
 * NOTE: The data buffer must be one byte longer than the actual data.
 *
 * Returns: 0 on success or negative error on parse failure.
 */
static int parse_conf(char *data, size_t size, struct label *label)
{
	char *command = NULL, *value = NULL;
	char *overlay, *saveptr;
	int cnt = 0;

	while (parse_command(&data, &size, &command, &value) == 0) {
		switch (cmd_to_tok(command)) {
			case CMD_KERNEL:
				label->kernel = value;
				break;
			case CMD_INITRD:
				label->initramfs = value;
				break;
			case CMD_APPEND:
				label->cmdline = value;
				break;
			case CMD_FDT:
				label->dtb = value;
				break;
			case CMD_FDTDIR:
				label->dtbdir = value;
				break;
			case CMD_FDTOVERLAY:
				for (char *c = value; *c; c++)
					if (*c == ' ')
						cnt++;

				cnt += 2;

				label->dtboverlays = calloc(sizeof(*label->dtboverlays), cnt);
				cnt = 0;
				for (overlay = strtok_r(value, " ", &saveptr); overlay;
				     overlay = strtok_r(NULL,  " ", &saveptr)) {
					label->dtboverlays[cnt] = overlay;
					cnt++;
				}
				break;
			default:
				break;
		}
	}

	return 0;
}

static bool fs_file_exists(const char *file)
{
	struct filehandle *fileh;
	int ret;

	if (!file)
		return false;

	ret = fs_open_file(file, &fileh);
	if (ret < 0)
		return false;

	fs_close_file(fileh);
	return true;
}

/**
 * expand_conf() - Sanity check and rewrite the parsed config.
 *
 * This function checks if all the values in the config are sane,
 * all mentioned files exists. It then appends the paths with the
 * root directory and rewrites the dtb field based on dtbdir if
 * possible. This funtion allocates new strings for all values.
 *
 * Returns: True if the config seems bootable, false otherwise.
 */
static bool expand_conf(struct label *label, const char *root)
{
	const char *const *dtbfiles = lk2nd_device_get_dtb_hints();
	char path[128];
	int i = 0;

	/* Cant boot without any kernel. */
	if (!label->kernel) {
		dprintf(INFO, "Kernel is not specified\n");
		return false;
	}

	snprintf(path, sizeof(path), "%s/%s", root, label->kernel);
	label->kernel = strndup(path, sizeof(path));

	if (!fs_file_exists(label->kernel)) {
		dprintf(INFO, "Kernel %s does not exist\n", label->kernel);
		return false;
	}

	/* lk2nd needs to patch the dtb to boot. */
	if (!label->dtbdir && !label->dtb) {
		dprintf(INFO, "Neither fdt nor fdtdir is specified\n");
		return false;
	}

	if (label->dtbdir) {
		if (!dtbfiles) {
			dprintf(INFO, "The dtb-files for this device is not set\n");
			return false;
		}

		while (dtbfiles[i]) {
			/* NOTE: Try aarch64 path, then aarch32 one. */
			snprintf(path, sizeof(path), "%s/%s/qcom/%s.dtb", root, label->dtbdir, dtbfiles[i]);
			if (!fs_file_exists(path))
				snprintf(path, sizeof(path), "%s/%s/qcom-%s.dtb", root, label->dtbdir, dtbfiles[i]);

			/* boot-deploy drops the vendor dir when copying dtbs. */
			if (!fs_file_exists(path))
				snprintf(path, sizeof(path), "%s/%s/%s.dtb", root, label->dtbdir, dtbfiles[i]);

			if (fs_file_exists(path)) {
				label->dtb = strndup(path, sizeof(path));
				break;
			}
			i++;
		}
	} else {
		snprintf(path, sizeof(path), "%s/%s", root, label->dtb);
		label->dtb = strndup(path, sizeof(path));
	}

	if (!fs_file_exists(label->dtb)) {
		dprintf(INFO, "FDT %s does not exist\n", label->dtb);
		return false;
	}

	if (label->dtboverlays) {
		i = 0;
		while (label->dtboverlays[i]) {
			snprintf(path, sizeof(path), "%s/%s", root, label->dtboverlays[i]);
			if (fs_file_exists(path)) {
				label->dtboverlays[i] = strndup(path, sizeof(path));
			} else {
				dprintf(INFO, "FDT overlay %s does not exist\n", label->dtboverlays[i]);
				return false;
			}
			i++;
		}
	}

	if (label->initramfs) {
		snprintf(path, sizeof(path), "%s/%s", root, label->initramfs);
		label->initramfs = strndup(path, sizeof(path));

		if (!fs_file_exists(label->initramfs)) {
			dprintf(INFO, "Initramfs %s does not exist\n", label->initramfs);
			return false;
		}
	}

	if (label->cmdline)
		label->cmdline = strdup(label->cmdline);
	else
		label->cmdline = "";

	return true;
}

extern void boot_linux(void *kernel, unsigned *tags,
		const char *cmdline, unsigned machtype,
		void *ramdisk, unsigned ramdisk_size,
		enum boot_type boot_type);

#define IS_ARM64(ptr) (ptr->magic_64 == KERNEL64_HDR_MAGIC)

#define MAX_KERNEL_SIZE			(32 * 1024 * 1024)
#define MAX_TAGS_SIZE			(2 * 1024 * 1024)
#define MAX_RAMDISK_SIZE		(16 * 1024 * 1024)

struct load_addrs {
	void *kernel;
	void *tags;
	void *ramdisk;
	uint32_t kernel_max_size;
	uint32_t ramdisk_max_size;
};

static void choose_addrs(const struct kernel64_hdr *kptr, struct load_addrs *addrs)
{
	uint32_t kernel_offset;
	void *base;

#ifdef DDR_START
	base = (void *)(uintptr_t)DDR_START;
#else
	base = (void *)(uintptr_t)BASE_ADDR;
#endif

	/* FIXME: Find a better approach to allow larger ramdisks */
	addrs->tags = base + MAX_KERNEL_SIZE;
	addrs->ramdisk = addrs->tags + MAX_TAGS_SIZE;
	addrs->ramdisk_max_size = MAX_RAMDISK_SIZE;

	/*
	 * ARM64 kernels specify the expected text offset in kptr->text_offset.
	 * However, this is not reliable until Linux 3.17. Check if
	 * image_size != 0 to detect newer kernels and use their expected offset.
	 *
	 * See Linux commit a2c1d73b94ed49f5fac12e95052d7b140783f800.
	 */
	if (IS_ARM64(kptr)) {
		if (kptr->image_size)
			kernel_offset = kptr->text_offset;
		else
			kernel_offset = 0x80000;

		/* U-Boot builds bogus .text_offset for some reason... */
		if (kptr->text_offset > 2 * 1024 * 1024) {
			dprintf(INFO, "WARNING: Kernel image has bogus text_offset: 0x%llx > 2M\n", kptr->text_offset);
			kernel_offset = 0;
		}
	} else {
		kernel_offset = 0x8000;
	}

	addrs->kernel = base + kernel_offset;
	addrs->kernel_max_size = MAX_KERNEL_SIZE - kernel_offset;
}

/**
 * lk2nd_boot_label() - Load all files from the label and boot.
 */
static void lk2nd_boot_label(struct label *label)
{
	unsigned int scratch_size = target_get_max_flash_size();
	void *scratch = target_get_scratch_address();
	unsigned int kernel_size, ramdisk_size = 0;
	struct load_addrs addrs;
	void *kernel_scratch;
	int ret, i = 0;

	ret = fs_load_file(label->kernel, scratch, scratch_size);
	if (ret < 0) {
		dprintf(INFO, "Failed to load the kernel: %d\n", ret);
		return;
	}

	kernel_size = ((ret >> 4) + 1) << 4;

	if (is_gzip_package(scratch, kernel_size)) {
		dprintf(INFO, "Decompressing the kernel...\n");
		kernel_scratch = scratch + kernel_size;
		ret = decompress(scratch, kernel_size, kernel_scratch,
				 scratch_size - kernel_size, NULL, &kernel_size);
		if (ret) {
			dprintf(INFO, "Failed to decompress the kernel: %d\n", ret);
			return;
		}
	} else {
		kernel_scratch = scratch;
	}

	choose_addrs(kernel_scratch, &addrs);

	if (kernel_size > addrs.kernel_max_size) {
		dprintf(INFO, "Kernel too big: %u > %u\n",
			kernel_size, addrs.kernel_max_size);
		return;
	}
	memmove(addrs.kernel, kernel_scratch, kernel_size);

	ret = fs_load_file(label->dtb, addrs.tags, MAX_TAGS_SIZE);
	if (ret < 0) {
		dprintf(INFO, "Failed to load the dtb: %d\n", ret);
		return;
	}
	if (ret == MAX_TAGS_SIZE) {
		dprintf(INFO, "DTB is too big\n");
		return;
	}

	if (label->dtboverlays) {
		ret = fdt_open_into(addrs.tags, addrs.tags, MAX_TAGS_SIZE);
		if (ret < 0) {
			dprintf(INFO, "Failed to open the dtb: %d\n", ret);
			return;
		}

		while (label->dtboverlays[i]) {
			ret = fs_load_file(label->dtboverlays[i], scratch, scratch_size);
			if (ret < 0) {
				dprintf(INFO, "Failed to load the dtb overlay %s: %d\n", label->dtboverlays[i], ret);
				return;
			}

			ret = fdt_overlay_apply(addrs.tags, scratch);
			if (ret < 0) {
				dprintf(INFO, "Failed to apply the dtb overlay %s: %d\n", label->dtboverlays[i], ret);
				return;
			}
			i++;
		}

		ret = fdt_pack(addrs.tags);
		if (ret < 0) {
			dprintf(INFO, "Failed to pack the dtb: %d\n", ret);
			return;
		}
	}

	if (label->initramfs) {
		ret = fs_load_file(label->initramfs, addrs.ramdisk, addrs.ramdisk_max_size);
		if (ret < 0) {
			dprintf(INFO, "Failed to load the initramfs: %d\n", ret);
			return;
		}
		if ((uint32_t)ret == addrs.ramdisk_max_size) {
			dprintf(INFO, "Initramfs is too big\n");
			return;
		}
		ramdisk_size = ret;
		arch_clean_invalidate_cache_range((addr_t)addrs.ramdisk, ramdisk_size);
	}

	boot_linux(addrs.kernel,
		   addrs.tags,
		   label->cmdline,
		   board_machtype(),
		   addrs.ramdisk, ramdisk_size,
		   0);
}

/**
 * lk2nd_try_extlinux() - Try to boot with extlinux
 *
 * Check if /extlinux/extlinux.conf exists and try to
 * boot it if so.
 */
void lk2nd_try_extlinux(const char *root)
{
	struct filehandle *fileh;
	struct file_stat stat;
	struct label label = {0};
	char path[64];
	char *data;
	int ret;

	snprintf(path, sizeof(path), "%s/extlinux/extlinux.conf", root);
	ret = fs_open_file(path, &fileh);
	if (ret < 0) {
		dprintf(SPEW, "No extlinux config in %s: %d\n", root, ret);
		return;
	}

	fs_stat_file(fileh, &stat);
	data = malloc(stat.size + 1);
	fs_read_file(fileh, data, 0, stat.size);
	fs_close_file(fileh);

	ret = parse_conf(data, stat.size, &label);
	if (ret < 0)
		goto error;

	if (!expand_conf(&label, root))
		goto error;

	free(data);

	dprintf(SPEW, "Parsed %s\n", path);
	dprintf(SPEW, "kernel    = %s\n", label.kernel);
	dprintf(SPEW, "dtb       = %s\n", label.dtb);
	dprintf(SPEW, "dtbdir    = %s\n", label.dtbdir);
	dprintf(SPEW, "initramfs = %s\n", label.initramfs);
	dprintf(SPEW, "cmdline   = %s\n", label.cmdline);

	lk2nd_boot_label(&label);

	return;

error:
	dprintf(INFO, "Failed to parse extlinux.conf\n");
	free(data);
}
