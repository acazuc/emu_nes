#include "mem.h"
#include "mbc.h"
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>

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
		return mem->wram[addr];
	}
	if (addr < 0x4000)
	{
		addr &= 7;
		switch (addr)
		{
			case 0x0:
			case 0x1:
			case 0x3:
			case 0x5:
			case 0x6:
				printf("read from RO gpu register 0x200%" PRIx16 "\n", addr);
				return 0;
			case 0x7:
			{
				uint8_t v = mem_gpu_get(mem, mem->gpu_regs[0x6]);
				mem->gpu_regs[0x6] += (mem->gpu_regs[0x0] & 0x40) ? 32 : 1;
				return v;
			}
			default:
				return mem->gpu_regs[addr];
		}
	}
	if (addr < 0x4018)
	{
		/* XXX APU IO */
		return 0;
	}
	return mbc_get(mem->mbc, addr);
}

void mem_set(mem_t *mem, uint16_t addr, uint8_t v)
{
	if (addr < 0x2000)
	{
		addr &= 0x7FF;
		mem->wram[addr] = v;
		return;
	}
	if (addr < 0x4000)
	{
		addr &= 0x7;
		switch (addr)
		{
			case 0x0:
			case 0x1:
			case 0x3:
			case 0x4:
				mem->gpu_regs[addr] = v;
				return;
			case 0x5:
			case 0x6:
				/* XXX */
				return;
			case 0x2:
				printf("write to RO gpu register 0x200%" PRIx16 "\n", addr);
				return;
			case 0x7:
				mem_gpu_set(mem, mem->gpu_regs[0x6], v);
				mem->gpu_regs[0x6] += (mem->gpu_regs[0x0] & 0x40) ? 32 : 1;
				return;
		}
		/* XXX PPU IO */
		return;
	}
	if (addr < 0x4018)
	{
		/* XXX APU IO */
		return;
	}
	mbc_set(mem->mbc, addr, v);
}

uint8_t mem_gpu_get(mem_t *mem, uint16_t addr)
{
	switch (addr >> 12)
	{
		case 0x0:
			return mem->gpu_pattern0[addr];
		case 0x1:
			return mem->gpu_pattern1[addr - 0x1000];
		case 0x2:
			return mem->gpu_names[addr - 0x2000];
		case 0x3:
			if (addr < 0x3EFF)
				return mem->gpu_names[addr - 0x3000];
			return mem->gpu_palettes[(addr - 0x3F00) & 0x1F];
		default:
			printf("get unknown gpu memory %04" PRIx16 "\n", addr);
			return 0;
	}
}

void mem_gpu_set(mem_t *mem, uint16_t addr, uint8_t v)
{
	switch (addr >> 12)
	{
		case 0x0:
			mem->gpu_pattern0[addr] = v;
			return;
		case 0x1:
			mem->gpu_pattern1[addr - 0x1000] = v;
			return;
		case 0x2:
			mem->gpu_names[addr - 0x2000] = v;
			return;
		case 0x3:
			if (addr < 0x3EFF)
				mem->gpu_names[addr - 0x3000] = v;
			else
				mem->gpu_palettes[(addr - 0x3F00) & 0x1F] = v;
			return;
		default:
			printf("set unknown gpu memory %04" PRIx16 " = %02" PRIx8 "\n",
			       addr, v);
			return;
	}
}
