#include "gpu.h"
#include "mem.h"
#include "nes.h"
#include "cpu.h"
#include <stdlib.h>

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
		uint16_t base = 0;//(mem_get_gpu_reg(gpu->mem, MEM_REG_GPU_RC1) & 0x10) ? 0x1000 : 0;
		//printf("addr[%u, %u] = %x\n", gpu->x, gpu->y, gpu->x / 8 + gpu->y / 8 * 32);
		uint8_t v = gpu->mem->gpu_pattern0[base + gpu->x / 8 + gpu->y / 8 * 32];
		v = v | (v << 6);
		uint32_t idx = (gpu->x + gpu->y * 256) * 4;
		gpu->data[idx + 0] = v;
		gpu->data[idx + 1] = v;
		gpu->data[idx + 2] = v;
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
	mem_set_gpu_reg(gpu->mem, MEM_REG_GPU_STATUS, 0x80);
}

void gpu_clock(gpu_t *gpu)
{
	if (++gpu->clock_count == 4) /* 5 in PAL */
	{
		gpu_cycle(gpu);
		gpu->clock_count = 0;
	}
}
