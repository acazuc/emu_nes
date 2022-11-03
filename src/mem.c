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
#if 0
		printf("get wram[0x%04" PRIx16 "] = %02" PRIx8 "\n", addr, mem->wram[addr]);
#endif
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
			case 0x4:
				return mem_gpu_get(mem, mem->spram_addr);
			case 0x7:
			{
				uint8_t v = mem_gpu_get(mem, mem->vram_addr);
				mem->vram_addr += (mem->gpu_regs[0x0] & 0x40) ? 32 : 1;
				mem->vram_addr &= 0x3FFF;
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
#if 0
		printf("set wram[0x%04" PRIx16 "] = %02" PRIx8 "\n", addr, v);
#endif
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
				mem->gpu_regs[addr] = v;
				return;
			case 0x5:
				/* XXX */
				return;
			case 0x3:
				mem->spram_addr = v;
				return;
			case 0x4:
				mem_gpu_set(mem, mem->spram_addr, v);
				mem->spram_addr++;
				return;
			case 0x6:
#if 1
				printf("writing VRAM addr %02" PRIx8 " (%d)\n", v, mem->vram_ff);
#endif
				if (mem->vram_ff)
					mem->vram_addr = (mem->vram_addr & 0x3F00) | v;
				else
					mem->vram_addr = (mem->vram_addr & 0xFF)
					               | ((uint16_t)(v & 0x3F) << 8);
				mem->vram_ff = !mem->vram_ff;
				return;
			case 0x2:
#if 0
				printf("write to RO gpu register 0x200%" PRIx16 "\n", addr);
#endif
				return;
			case 0x7:
				mem_gpu_set(mem, mem->vram_addr, v);
				mem->vram_addr += (mem->gpu_regs[0x0] & 0x40) ? 32 : 1;
				mem->vram_addr &= 0x3FFF;
				return;
		}
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
			/* XXX to mbc */
			return mem->gpu_pattern0[addr];
		case 0x1:
			/* XXX to mbc */
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
#if 0
	printf("gpu set [0x%04" PRIx16 "] = %02" PRIx8 "\n", addr, v);
#endif
	switch (addr >> 12)
	{
		case 0x0:
			/* XXX to mbc */
			mem->gpu_pattern0[addr] = v;
			return;
		case 0x1:
			/* XXX to mbc */
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
