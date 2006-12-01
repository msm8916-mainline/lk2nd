#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <fdt.h>
#include <libfdt_env.h>

#include "testdata.h"

struct {
	struct fdt_header *fdt;
	const char *filename;
} trees[] = {
#define TREE(name)	{ &_##name, #name ".dtb" }
	TREE(test_tree1),
};

#define NUM_TREES	(sizeof(trees) / sizeof(trees[0]))

int main(int argc, char *argv[])
{
	int i;

	for (i = 0; i < NUM_TREES; i++) {
		struct fdt_header *fdt = trees[i].fdt;
		const char *filename = trees[i].filename;
		int size;
		int fd;
		int ret;

		size = fdt32_to_cpu(fdt->totalsize);

		printf("Tree \"%s\", %d bytes\n", filename, size);

		fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0666);
		if (fd < 0)
			perror("open()");

		ret = write(fd, fdt, size);
		if (ret != size)
			perror("write()");

		close(fd);
	}
	exit(0);
}
