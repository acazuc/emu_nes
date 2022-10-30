#ifndef MEM_H
#define MEM_H

#include <stdint.h>

typedef struct mbc mbc_t;

typedef struct mem
{
	mbc_t *mbc;
} mem_t;

mem_t *mem_new(mbc_t *mbc);
void mem_del(mem_t *mem);

uint8_t mem_get(mem_t *mem, uint16_t addr);
void mem_set(mem_t *mem, uint16_t addr, uint8_t v);

#endif
