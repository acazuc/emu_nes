#include "gpu.h"
#include "mem.h"
#include "nes.h"
#include "cpu.h"
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>

gpu_t *gpu_new(nes_t *nes, mem_t *mem)
{
	gpu_t *gpu = calloc(sizeof(*gpu), 1);
	if (!gpu)
		return NULL;
	gpu->nes = nes;
	gpu->mem = mem;
	return gpu;
}

void gpu_del(gpu_t *gpu)
{
	if (!gpu)
		return;
	free(gpu);
}

static void gpu_cycle(gpu_t *gpu)
{
	if (gpu->x < 256 && gpu->y < 240)
	{
		uint8_t bx = gpu->x / 8;
		uint8_t by = gpu->y / 8;
		uint8_t px = gpu->x % 8;
		uint8_t py = gpu->y % 8;
		uint16_t chr = mem_gpu_get(gpu->mem, 0x2000 + bx + by * 32);
		uint8_t palid = mem_gpu_get(gpu->mem, 0x23C0 + bx / 4 + by / 4 * 8);
		uint8_t pals = (bx & 2) + ((by & 2) << 1);
		palid = (palid >> pals) & 0x3;
		uint16_t addr = (chr * 16) & 0xFFF;
		if (mem_get_gpu_reg(gpu->mem, MEM_REG_GPU_RC1) & 0x10)
			addr += 0x1000;
		uint8_t v1 = mem_gpu_get(gpu->mem, addr + py + 0);
		uint8_t v2 = mem_gpu_get(gpu->mem, addr + py + 8);
		uint8_t v = (((v1 >> (7 - px)) & 1) << 0)
		          | (((v2 >> (7 - px)) & 1) << 1);
#if 1
		if (v)
			v = mem_gpu_get(gpu->mem, 0x3F00 | (palid << 2) | v);
		else
			v = mem_gpu_get(gpu->mem, 0x3F00);
#else
		v = chr;
#endif
		static const uint8_t colors[3 * 0x40] =
		{
			124,124,124,
			0,0,252,
			0,0,188,
			68,40,188,
			148,0,132,
			168,0,32,
			168,16,0,
			136,20,0,
			80,48,0,
			0,120,0,
			0,104,0,
			0,88,0,
			0,64,88,
			0,0,0,
			0,0,0,
			0,0,0,
			188,188,188,
			0,120,248,
			0,88,248,
			104,68,252,
			216,0,204,
			228,0,88,
			248,56,0,
			228,92,16,
			172,124,0,
			0,184,0,
			0,168,0,
			0,168,68,
			0,136,136,
			0,0,0,
			0,0,0,
			0,0,0,
			248,248,248,
			60,188,252,
			104,136,252,
			152,120,248,
			248,120,248,
			248,88,152,
			248,120,88,
			252,160,68,
			248,184,0,
			184,248,24,
			88,216,84,
			88,248,152,
			0,232,216,
			120,120,120,
			0,0,0,
			0,0,0,
			252,252,252,
			164,228,252,
			184,184,248,
			216,184,248,
			248,184,248,
			248,164,192,
			240,208,176,
			252,224,168,
			248,216,120,
			216,248,120,
			184,248,184,
			184,248,216,
			0,252,252,
			248,216,248,
			0,0,0,
			0,0,0,
		};
		uint32_t idx = (gpu->x + gpu->y * 256) * 4;
#if 1
		uint8_t col = 3 * (v & 0x3F);
		gpu->data[idx + 0] = colors[col + 0];
		gpu->data[idx + 1] = colors[col + 1];
		gpu->data[idx + 2] = colors[col + 2];
#else
		gpu->data[idx + 0] = v;
		gpu->data[idx + 1] = v;
		gpu->data[idx + 2] = v;
#endif
		gpu->data[idx + 3] = 0xff;
	}
	gpu->x++;
	if (gpu->x == 341)
	{
		gpu->x = 0;
		gpu->y++;
		if (gpu->y == 240)
		{
			if (mem_get_gpu_reg(gpu->mem, MEM_REG_GPU_RC1) & 0x80)
				cpu_nmi(gpu->nes->cpu);
		}
		if (gpu->y == 262)
		{
			gpu->y = 0;
		}
	}
	if (gpu->y >= 240)
		mem_set_gpu_reg(gpu->mem, MEM_REG_GPU_STATUS, 0x80);
	else
		mem_set_gpu_reg(gpu->mem, MEM_REG_GPU_STATUS, 0x00);
}

void gpu_clock(gpu_t *gpu)
{
	if (++gpu->clock_count == 4) /* 5 in PAL */
	{
		gpu_cycle(gpu);
		gpu->clock_count = 0;
	}
}
