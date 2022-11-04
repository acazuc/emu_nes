#include "mbc.h"
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

mbc_t *mbc_new(const void *data, size_t size)
{
	mbc_t *mbc = calloc(sizeof(*mbc), 1);
	if (!mbc)
		return NULL;
	if (size < sizeof(struct ines))
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
	mbc->ines = (struct ines*)mbc->data;
	if (mbc->ines->flags6 & (1 << 2))
		mbc->trainer = &mbc->data[16];
	else
		mbc->trainer = NULL;

	mbc->prg_rom_data = mbc->trainer ? &mbc->data[528] : &mbc->data[16];
	mbc->prg_rom_size = 16384 << (mbc->ines->prg_rom_lsb - 1);
	if (mbc->ines->chr_rom_lsb)
	{
		mbc->chr_rom_size = 8192 << (mbc->ines->chr_rom_lsb - 1);
		mbc->chr_rom_data = &mbc->prg_rom_data[mbc->prg_rom_size];
	}
	else
	{
		mbc->chr_rom_size = 0;
		mbc->chr_rom_data = NULL;
	}
	printf("prg_rom_size: %" PRIx16 "\n", (uint16_t)mbc->prg_rom_size);
	printf("prg_rom_data: %" PRIx32 "\n", (uint32_t)(mbc->prg_rom_data - mbc->data));
	printf("chr_rom_size: %" PRIx16 "\n", (uint16_t)mbc->chr_rom_size);
	printf("chr_rom_data: %" PRIx32 "\n", (uint32_t)(mbc->chr_rom_data - mbc->data));
	return mbc;
}

void mbc_del(mbc_t *mbc)
{
	if (!mbc)
		return;
	free(mbc->data);
	free(mbc);
}

static uint8_t mmc0_get(mbc_t *mbc, uint16_t addr)
{
	if (addr < 0x6000)
		return 0;
	if (addr < 0x8000)
		return 0;
	if (addr < 0xC000)
	{
		addr -= 0x8000;
		if (addr >= mbc->prg_rom_size)
			return 0;
		return mbc->prg_rom_data[addr];
	}
	if (mbc->ines->prg_rom_lsb > 1)
		addr -= 0x8000;
	else
		addr -= 0xC000;
	if (addr >= mbc->prg_rom_size)
		return 0;
	return mbc->prg_rom_data[addr];
}

static void mmc0_set(mbc_t *mbc, uint16_t addr, uint8_t v)
{
	(void)mbc;
	(void)addr;
	(void)v;
}

static uint8_t mmc0_gpu_get(mbc_t *mbc, uint16_t addr)
{
	if (addr < 0x2000)
	{
		if (addr >= mbc->chr_rom_size)
			return 0;
		return mbc->chr_rom_data[addr];
	}
	return 0;
}

static void mmc0_gpu_set(mbc_t *mbc, uint16_t addr, uint8_t v)
{
	(void)mbc;
	(void)addr;
	(void)v;
}

static uint8_t mmc1_get(mbc_t *mbc, uint16_t addr)
{
	if (addr < 0x6000)
		return 0;
	if (addr < 0x8000)
		return 0; /* XXX PRG RAM */
	if (addr < 0xC000)
	{
		/* XXX bank */
		addr -= 0x8000;
		if (addr >= mbc->prg_rom_size)
			return 0;
		return mbc->prg_rom_data[addr];
	}
	/* XXX bank */
	addr -= 0xC000;
	if (addr >= mbc->prg_rom_size)
		return 0;
	return mbc->prg_rom_data[addr];
}

static void mmc1_set(mbc_t *mbc, uint16_t addr, uint8_t v)
{
}

static uint8_t mmc1_gpu_get(mbc_t *mbc, uint16_t addr)
{
	if (addr < 0x2000)
	{
		if (addr >= mbc->chr_rom_size)
			return 0;
		return mbc->chr_rom_data[addr];
	}
	return 0;
}

static void mmc1_gpu_set(mbc_t *mbc, uint16_t addr, uint8_t v)
{
	(void)mbc;
	(void)addr;
	(void)v;
}

uint8_t mbc_get(mbc_t *mbc, uint16_t addr)
{
	switch (mbc->ines->flags6 >> 4)
	{
		case 0:
			return mmc0_get(mbc, addr);
		case 1:
			return mmc1_get(mbc, addr);
		default:
			return 0;
	}
}

void mbc_set(mbc_t *mbc, uint16_t addr, uint8_t v)
{
	switch (mbc->ines->flags6 >> 4)
	{
		case 0:
			mmc0_set(mbc, addr, v);
			break;
		case 1:
			mmc1_set(mbc, addr, v);
			break;
		default:
			return;
	}
}

uint8_t mbc_gpu_get(mbc_t *mbc, uint16_t addr)
{
	switch (mbc->ines->flags6 >> 4)
	{
		case 0:
			return mmc0_gpu_get(mbc, addr);
		case 1:
			return mmc1_gpu_get(mbc, addr);
		default:
			return 0;
	}
}

void mbc_gpu_set(mbc_t *mbc, uint16_t addr, uint8_t v)
{
	switch (mbc->ines->flags6 >> 4)
	{
		case 0:
			mmc0_gpu_set(mbc, addr, v);
			break;
		case 1:
			mmc1_gpu_set(mbc, addr, v);
			break;
		default:
			return;
	}
}
