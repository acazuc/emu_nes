#ifndef GPU_H
#define GPU_H

#include <stdint.h>

typedef struct mem mem_t;
typedef struct nes nes_t;

typedef struct gpu
{
	nes_t *nes;
	mem_t *mem;
	uint8_t clock_count;
	uint16_t x;
	uint16_t y;
} gpu_t;

gpu_t *gpu_new(nes_t *nes, mem_t *mem);
void gpu_del(gpu_t *gpu);
void gpu_clock(gpu_t *gpu);

#endif
