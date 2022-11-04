#ifndef MEM_H
#define MEM_H

#include <stdint.h>

#define MEM_REG_GPU_RC1          0x2000
#define MEM_REG_GPU_RC2          0x2001
#define MEM_REG_GPU_STATUS       0x2002
#define MEM_REG_GPU_SPR_RAM_ADDR 0x2003
#define MEM_REG_GPU_SPR_RAM_DATA 0x2004
#define MEM_REG_GPU_SCROLL       0x2005
#define MEM_REG_GPU_VRAM_ADDR    0x2006
#define MEM_REG_GPU_VRAM_DATA    0x2007

typedef struct mbc mbc_t;

typedef struct mem
{
	mbc_t *mbc;
	uint8_t gpu_regs[7];
	uint8_t wram[0x800];
	uint8_t gpu_names[0x1000];
	uint8_t gpu_palettes[0x20];
	uint16_t vram_addr;
	uint8_t spram_addr;
	uint8_t vram_ff;
} mem_t;

mem_t *mem_new(mbc_t *mbc);
void mem_del(mem_t *mem);

uint8_t mem_get(mem_t *mem, uint16_t addr);
void mem_set(mem_t *mem, uint16_t addr, uint8_t v);

uint8_t mem_gpu_get(mem_t *mem, uint16_t addr);
void mem_gpu_set(mem_t *mem, uint16_t addr, uint8_t v);

static inline void mem_set_gpu_reg(mem_t *mem, uint16_t r, uint8_t v)
{
	mem->gpu_regs[r - 0x2000] = v;
}

static inline uint8_t mem_get_gpu_reg(mem_t *mem, uint16_t r)
{
	return mem->gpu_regs[r - 0x2000];
}

#endif
