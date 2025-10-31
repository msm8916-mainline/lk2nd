// SPDX-License-Identifier: BSD-3-Clause
/* Copyright (c) 2023 Nikita Travkin <nikita@trvn.ru> */

#include <debug.h>
#include <decompress.h>
#include <lib/fs.h>
#include <libfdt.h>
#include <platform.h>
#include <platform/iomap.h>
#include <smem.h>
#include <strings.h>
#include <stdlib.h>
#include <target.h>

#include "../../app/aboot/bootimg.h"

#include <lk2nd/boot.h>
#include <lk2nd/device.h>

#include "boot.h"

enum token {
	CMD_LABEL,
	CMD_DEFAULT,
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
	{"label",		CMD_LABEL},
	{"default",		CMD_DEFAULT},
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
		if (!strcasecmp(command, token_map[i].command))
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
 * count_lines() - Count the amount of lines in the string.
 */
static int count_lines(char *data, size_t size)
{
	int acc = 0;
	size_t i;

	for (i = 0; i < size; ++i) {
		if (data[i] == '\n')
			acc++;

		if (data[i] == '\0')
			break;
	}

	return acc;
}

static void copy_label_data(struct label *label) {
	if (label->name)
		label->name = strdup(label->name);
	if (label->kernel)
		label->kernel = strdup(label->kernel);
	if (label->initramfs)
		label->initramfs = strdup(label->initramfs);
	if (label->dtb)
		label->dtb = strdup(label->dtb);
	if (label->dtbdir)
		label->dtbdir = strdup(label->dtbdir);
	if (label->cmdline)
		label->cmdline = strdup(label->cmdline);
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
static int parse_conf(char *data, size_t size, struct label **labels_out, int* default_label_idx, int* labels_count_out)
{
	char *command = NULL, *value = NULL;
	char *overlay, *saveptr;
	int cnt = 0;
	struct {
		enum token cmd;
		char *val;
	} *commands;
	int commands_count;
	struct label *labels;
	const char *default_name = "";
	int labels_count = 0;
	int label_idx;
	int i;

	commands_count = count_lines(data, size);
	commands = calloc(commands_count, sizeof(*commands));

	i = 0;
	while (parse_command(&data, &size, &command, &value) == 0) {
		if (i >= commands_count) {
			dprintf(INFO, "Failed to parse the extlinux.conf\n");
			free(commands);
			return -1;
		}

		commands[i].cmd = cmd_to_tok(command);
		commands[i].val = value;
		i++;
	}

	commands_count = i;

	i = 0;
	for (i = 0; i < commands_count; ++i) {
		if (commands[i].cmd == CMD_LABEL)
			labels_count++;
	}

	if (labels_count == 0) {
		dprintf(INFO, "No labels in the extlinux.conf\n");
		free(commands);
		return -1;
	}

	labels = calloc(labels_count, sizeof(*labels));

	label_idx = -1;
	for (i = 0; i < commands_count; ++i) {
		if (commands[i].cmd == CMD_DEFAULT) {
			default_name = commands[i].val;
		} else if (commands[i].cmd == CMD_LABEL) {
			label_idx++;
			labels[label_idx].name = commands[i].val;
		} else if (label_idx >= 0 && label_idx < labels_count) {
			switch (commands[i].cmd) {
			case CMD_KERNEL:
				labels[label_idx].kernel = commands[i].val;
				break;
			case CMD_INITRD:
				labels[label_idx].initramfs = commands[i].val;
				break;
			case CMD_APPEND:
				labels[label_idx].cmdline = commands[i].val;
				break;
			case CMD_FDT:
				labels[label_idx].dtb = commands[i].val;
				break;
			case CMD_FDTDIR:
				labels[label_idx].dtbdir = commands[i].val;
				break;
			case CMD_FDTOVERLAY:
				for (char *c = commands[i].val; *c; c++)
					if (*c == ' ')
						cnt++;

				cnt += 2;

				labels[label_idx].dtboverlays = calloc(cnt, sizeof(*labels[label_idx].dtboverlays));
				cnt = 0;
				for (overlay = strtok_r(commands[i].val, " ", &saveptr); overlay;
				     overlay = strtok_r(NULL,  " ", &saveptr)) {
					labels[label_idx].dtboverlays[cnt] = overlay;
					cnt++;
				}
				break;
			default:
				break;
			}
		}
	}

	*default_label_idx = 0;

	for(i = 0; i < labels_count; i++) {
		copy_label_data(&labels[i]);
	}

	for (i = 0; i < labels_count; ++i) {
		if (!strcmp(default_name, labels[i].name)) {
			*default_label_idx = i;
			break;
		}
	}

	*labels_out = labels;
	*labels_count_out = labels_count;

	free(commands);

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
 * normalize_path() - Normalize path against given root.
 *
 * Given some path (absolute or relative), normalize it to
 * the lk "vfs" path; prepending /extlinux/ if the path is
 * relative.
 *
 * Returns: Newly allocated copy of the string with normalized
 * path.
 */
static char *normalize_path(const char *path, const char *root)
{
	char tmp[256];

	if (path[0] == '/')
		snprintf(tmp, sizeof(tmp), "%s/%s", root, path);
	else
		snprintf(tmp, sizeof(tmp), "%s/extlinux/%s", root, path);

	return strndup(tmp, sizeof(tmp));
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
	int i = 0;

	/* Cant boot without any kernel. */
	if (!label->kernel) {
		dprintf(INFO, "Kernel is not specified\n");
		return false;
	}

	label->kernel = normalize_path(label->kernel, root);

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
			char dtb[128];
			char *normalized = NULL;

			/* Try arm64 style path. */
			snprintf(dtb, sizeof(dtb), "%s/qcom/%s.dtb", label->dtbdir, dtbfiles[i]);
			normalized = normalize_path(dtb, root);
			if (fs_file_exists(normalized)) {
				label->dtb = normalized;
				break;
			}
			free(normalized);

			/* Try arm32 style path. */
			snprintf(dtb, sizeof(dtb), "%s/qcom-%s.dtb", label->dtbdir, dtbfiles[i]);
			normalized = normalize_path(dtb, root);
			if (fs_file_exists(normalized)) {
				label->dtb = normalized;
				break;
			}
			free(normalized);

			/* boot-deploy drops the vendor dir when copying dtbs. */
			snprintf(dtb, sizeof(dtb), "%s/%s.dtb", label->dtbdir, dtbfiles[i]);
			normalized = normalize_path(dtb, root);
			if (fs_file_exists(normalized)) {
				label->dtb = normalized;
				break;
			}
			free(normalized);

			i++;
		}
	} else {
		label->dtb = normalize_path(label->dtb, root);
	}

	if (!fs_file_exists(label->dtb)) {
		dprintf(INFO, "FDT %s does not exist\n", label->dtb);
		return false;
	}

	if (label->dtboverlays) {
		i = 0;
		while (label->dtboverlays[i]) {
			label->dtboverlays[i] = normalize_path(label->dtboverlays[i], root);
			if (!fs_file_exists(label->dtboverlays[i])) {
				dprintf(INFO, "FDT overlay %s does not exist\n", label->dtboverlays[i]);
				return false;
			}

			i++;
		}
	}

	if (label->initramfs) {
		label->initramfs = normalize_path(label->initramfs, root);

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

	dprintf(INFO, "Trying to boot '%s'\n", label->name);

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


int lk2nd_get_extlinux_labels(const char *root, struct label **labels, int *default_label_idx, int* labels_count)
{
	struct filehandle *fileh;
	struct file_stat stat;
	char path[64];
	char *data;
	int ret;

	snprintf(path, sizeof(path), "%s/extlinux/extlinux.conf", root);
	ret = fs_open_file(path, &fileh);
	if (ret < 0) {
		dprintf(SPEW, "No extlinux config in %s: %d\n", root, ret);
		return -1;
	}

	fs_stat_file(fileh, &stat);
	data = malloc(stat.size + 1);
	fs_read_file(fileh, data, 0, stat.size);
	fs_close_file(fileh);

	ret = parse_conf(data, stat.size, labels, default_label_idx, labels_count);
	if (ret < 0)
		goto error;

	dprintf(INFO, "count: %d\n", *labels_count);

	free(data);

	return 0;

error:
	dprintf(INFO, "Failed to parse %s\n", path);
	free(data);
	return -1;
}

int lk2nd_expand_conf_and_boot_label(struct label *label, const char *root)
{
	if (!expand_conf(label, root))
		return -1;
	dprintf(INFO, "kernel    = %s\n", label->kernel);
	dprintf(INFO, "dtb       = %s\n", label->dtb);
	dprintf(INFO, "dtbdir    = %s\n", label->dtbdir);
	dprintf(INFO, "initramfs = %s\n", label->initramfs);
	dprintf(INFO, "cmdline   = %s\n", label->cmdline);
	lk2nd_boot_label(label);
	return -1;
}

/**
 * lk2nd_try_extlinux() - Try to boot with extlinux
 *
 * Check if /extlinux/extlinux.conf exists and try to
 * boot it if so.
 */
void lk2nd_try_extlinux(const char *root)
{
	struct label *labels;
	struct label *label;
	char path[64];
	int ret;
	int default_label_idx;
	int labels_count;

	ret = lk2nd_get_extlinux_labels(root, &labels, &default_label_idx, &labels_count);
	if (ret < 0) {
		return;
	}

	label = &labels[default_label_idx];

	if (!expand_conf(label, root))
		goto error;

	dprintf(INFO, "Parsed %s\n", path);
	dprintf(INFO, "kernel    = %s\n", label->kernel);
	dprintf(INFO, "dtb       = %s\n", label->dtb);
	dprintf(INFO, "dtbdir    = %s\n", label->dtbdir);
	dprintf(INFO, "initramfs = %s\n", label->initramfs);
	dprintf(INFO, "cmdline   = %s\n", label->cmdline);

	lk2nd_boot_label(label);

	free(labels);
	return;

error:
	dprintf(INFO, "Failed to parse extlinux.conf\n");
}
