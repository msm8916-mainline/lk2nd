/* Copyright 2007, Google Inc. */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/stat.h>

int main(int argc, char *argv[])
{
	struct stat s;
	unsigned size, base;
	int unified_boot = 0;
	unsigned unified_boot_magic[20];
	unsigned non_unified_boot_magic[10];
	unsigned magic_len = 0;
	unsigned *magic;
	int fd;

	if(argc < 3) {
		fprintf(stderr,"usage: mkheader <bin> <hdr>\n");
		return -1;
	}

	if (argc == 4) {
		if(!strcmp("unified-boot",argv[3])) {
			unified_boot = 1;
		}
	}

	if(stat(argv[1], &s)) {
		perror("cannot stat binary");
		return -1;
	}

	if(unified_boot) {
		magic = unified_boot_magic;
		magic_len = sizeof(unified_boot_magic);
	} else {
		magic = non_unified_boot_magic;
		magic_len = sizeof(non_unified_boot_magic);
	}

	size = s.st_size;
	base = 0;

	magic[0] = 0x00000005; /* appsbl */
	magic[1] = 0x00000002; /* nand */
	magic[2] = 0x00000000;
	magic[3] = base;
	magic[4] = size;
	magic[5] = size;
	magic[6] = size + base;
	magic[7] = 0x00000000;
	magic[8] = size + base;
	magic[9] = 0x00000000;

	if (unified_boot == 1)
	{
		magic[10] = 0x33836685; /* cookie magic number */
		magic[11] = 0x00000001; /* cookie version */
		magic[12] = 0x00000002; /* file formats */
		magic[13] = 0x00000000;
		magic[14] = 0x00500000; /* 5M for boot.img */
		magic[15] = 0x00000000;
		magic[16] = 0x00000000;
		magic[17] = 0x00000000;
		magic[18] = 0x00000000;
		magic[19] = 0x00000000;
	}

	fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if(fd < 0) {
		perror("cannot open header for writing");
		return -1;
	}
	if(write(fd, magic, magic_len) != magic_len) {
		perror("cannot write header");
		close(fd);
		unlink(argv[2]);
		return -1;
	}
	close(fd);

	return 0;
}
