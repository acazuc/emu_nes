#ifndef GPU_H
#define GPU_H

#include <stdint.h>

typedef struct mem mem_t;

typedef struct gpu
{
	mem_t *mem;
	uint8_t clock_count;
} gpu_t;

gpu_t *gpu_new(mem_t *mem);
void gpu_del(gpu_t *gpu);
void gpu_clock(gpu_t *gpu);

#endif
