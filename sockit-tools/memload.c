/*
 * Copyright (C) 2013 Stefan Kristiansson, stefan.kristiansson@saunalahti.fi
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2.  This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */

#include <stdio.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static void swap(uint32_t *mem, int count)
{
	while (count--) {
		*mem =  (*mem << 24) & 0xff000000 |
			(*mem <<  8) & 0x00ff0000 |
			(*mem >>  8) & 0x0000ff00 |
			(*mem >> 24) & 0x000000ff;
		mem++;
	}
}

/* helper function for mmap */
static size_t map_mem(int fd, void **va, off_t pa, size_t length)
{
	off_t pa_aligned = pa & ~(sysconf(_SC_PAGE_SIZE)-1);
	length = pa - pa_aligned + length;
	*va = mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, pa_aligned);

	return length;
}

int main(int argc, char *argv[])
{
	int i;
	int fd;
	void *va;
	size_t filesize;
	off_t pa = 0xc0000000;
	size_t length;
	uint32_t *mem;
	FILE *binfile;
	char *filename;

	if (argc < 2) {
		printf("Usage: %s <binary file>\n", argv[0]);
		return 1;
	}

	filename = argv[1];

	/* Open binary file and get file size */
	binfile = fopen(filename, "rb");
	if (!binfile) {
		printf("Error opening %s\n", filename);
		return 1;
	}
	fseek(binfile, 0, SEEK_END);
	filesize = ftell(binfile);
	rewind(binfile);

	if ((fd = open("/dev/mem", O_RDWR, 0)) < 0) {
		printf("Error opening /dev/mem\n");
		return 1;
	}

	length = map_mem(fd, &va, pa, filesize);

	if (!va) {
		printf("mmap of 0x%08x failed\n", (unsigned int)va);
		return 1;
	}

	mem = (uint32_t *)(va + length - filesize);

	/* Re-calculate file size in 32-bit words */
	/* TODO: handle the case below correctly */
	if (filesize % sizeof(uint32_t))
		printf("Error: file size is not ending on a 32-bit boundary\n");

	filesize = filesize/sizeof(uint32_t);

	if (fread(mem, sizeof(uint32_t), filesize, binfile) != filesize) {
		printf("Error reading %s\n", filename);
	}

	/* Handle endian difference */
	swap(mem, filesize);

	fclose(binfile);
	munmap(va, length);
	close(fd);

	return 0;
}
