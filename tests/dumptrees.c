#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>

#include <fdt.h>
#include <libfdt.h>
#include <libfdt_env.h>

#include "testdata.h"

struct {
	void *blob;
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
		void *blob = trees[i].blob;
		const char *filename = trees[i].filename;
		int size;
		int fd;
		int ret;

		size = fdt_totalsize(blob);

		printf("Tree \"%s\", %d bytes\n", filename, size);

		fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0666);
		if (fd < 0)
			perror("open()");

		ret = write(fd, blob, size);
		if (ret != size)
			perror("write()");

		close(fd);
	}
	exit(0);
}
