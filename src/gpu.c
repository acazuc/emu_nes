#include "gpu.h"
#include <stdlib.h>

gpu_t *gpu_new(mem_t *mem)
{
	gpu_t *gpu = calloc(sizeof(*gpu), 1);
	if (!gpu)
		return NULL;
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
}

void gpu_clock(gpu_t *gpu)
{
	if (++gpu->clock_count == 4) /* 5 in PAL */
	{
		gpu_cycle(gpu);
		gpu->clock_count = 0;
	}
}
