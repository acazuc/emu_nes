#include "mem.h"
#include <stdlib.h>

mem_t *mem_new(mbc_t *mbc)
{
	mem_t *mem = calloc(sizeof(*mem), 1);
	if (!mem)
		return NULL;
	mem->mbc = mbc;
	return mem;
}

void mem_del(mem_t *mem)
{
	if (!mem)
		return;
	free(mem);
}

uint8_t mem_get(mem_t *mem, uint16_t addr)
{
	if (addr < 0x2000)
	{
		addr &= 0x7FF;
		/* XXX 2K WRAM */
		return 0;
	}
	if (addr < 0x4000)
	{
		addr &= 0x7;
		/* XXX PPU IO */
		return 0;
	}
	if (addr < 0x4018)
	{
		/* XXX APU IO */
		return 0;
	}
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
	/* XXX cartridge PRG-ROM */
	return 0;
}

void mem_set(mem_t *mem, uint16_t addr, uint8_t v)
{
	if (addr < 0x2000)
	{
		addr &= 0x7FF;
		/* XXX 2K WRAM */
		return;
	}
	if (addr < 0x4000)
	{
		addr &= 0x7;
		/* XXX PPU IO */
		return;
	}
	if (addr < 0x4018)
	{
		/* XXX APU IO */
		return;
	}
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
