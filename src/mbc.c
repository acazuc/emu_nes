#include "mbc.h"
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
	mbc->prg_rom_size = 16384 << mbc->ines->prg_rom_lsb;
	if (mbc->ines->chr_rom_lsb)
	{
		mbc->chr_rom_size = 8192 << mbc->ines->chr_rom_lsb;
		mbc->chr_rom_data = &mbc->prg_rom_data[mbc->prg_rom_size];
	}
	else
	{
		mbc->chr_rom_size = 0;
		mbc->chr_rom_data = NULL;
	}
	return mbc;
}

void mbc_del(mbc_t *mbc)
{
	if (!mbc)
		return;
	free(mbc->data);
	free(mbc);
}

uint8_t mbc_get(mbc_t *mbc, uint16_t addr)
{
	if (addr < 0x6000)
	{
		/* XXX cartridge expansion */
		return 0;
	}
	if (addr < 0x8000)
	{
		/* XXX cartridge SRAM */
		return 0;
	}
	addr -= 0x8000;
	if (addr >= mbc->prg_rom_size)
		return 0;
	return mbc->prg_rom_data[addr];
}

void mbc_set(mbc_t *mbc, uint16_t addr, uint8_t v)
{
	if (addr < 0x6000)
	{
		/* XXX cartridge expansion */
		return;
	}
	if (addr < 0x8000)
	{
		/* XXX cartridge SRAM */
		return;
	}
	/* XXX cartridge PRG-ROM */
}
