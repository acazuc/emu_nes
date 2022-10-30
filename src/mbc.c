#include "mbc.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

mbc_t *mbc_new(const void *data, size_t size)
{
	mbc_t *mbc = calloc(sizeof(*mbc), 1);
	if (!mbc)
		return NULL;
	if (size < 16)
	{
		fprintf(stderr, "invalid iNES header\n");
		return NULL;
	}
	if (memcmp(data, "NES\x1A", 4))
	{
		fprintf(stderr, "invalid iNES magic\n");
		return NULL;
	}
	mbc->data = malloc(size);
	if (!mbc->data)
	{
		free(mbc);
		return NULL;
	}
	memcpy(mbc->data, data, size);
	mbc->size = size;
	return mbc;
}

void mbc_del(mbc_t *mbc)
{
	if (!mbc)
		return;
	free(mbc->data);
	free(mbc);
}
