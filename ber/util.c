#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "util.h"

uint64_t slurp(char *path, uint8_t **dst, uint8_t *e) {
	const uint64_t block_size = 4096;
	uint64_t current = block_size;
	uint64_t total_read = 0, bytes_read = 0;
	uint8_t *tmp;
	FILE *f = fopen(path, "rb");
	if (!f) {
		*dst = NULL;
		*e = 1;
		return 0;
	}
	*dst = malloc(current);
	if (!*dst) {
		*e = 1;
		fclose(f);
		return 0;
	}
	while ((bytes_read = fread(*dst, 1, block_size, f)) == block_size) {
		total_read += block_size;
		current += block_size;
		tmp = realloc(*dst, current);
		if (!tmp) {
			*e = 1;
			fclose(f);
			return total_read;
		}
		*dst = tmp;
	}
	if (ferror(f))
		*e = 1;
	*e = 0;
	fclose(f);
	return total_read + bytes_read;
}
