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
	unsigned magic[10];
	int fd;

	if(argc != 3) {
		fprintf(stderr,"usage: mkheader <bin> <hdr>\n");
		return -1;
	}

	if(stat(argv[1], &s)) {
		perror("cannot stat binary");
		return -1;
	}

	size = s.st_size;
	base = 0xE0000000;

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

	fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if(fd < 0) {
		perror("cannot open header for writing");
		return -1;
	}
	if(write(fd, magic, sizeof(magic)) != sizeof(magic)) {
		perror("cannot write header");
		close(fd);
		unlink(argv[2]);
		return -1;
	}
	close(fd);

	return 0;
}
