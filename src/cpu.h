#ifndef CPU_H
#define CPU_H

#include <stdint.h>

typedef struct mem mem_t;

enum cpu_flags
{
	CPU_FLAG_C = (1 << 0),
	CPU_FLAG_Z = (1 << 1),
	CPU_FLAG_I = (1 << 2),
	CPU_FLAG_D = (1 << 3),
	CPU_FLAG_B = (1 << 4),
	CPU_FLAG_V = (1 << 6),
	CPU_FLAG_N = (1 << 7),
};

typedef struct cpu_regs
{
	uint8_t a;
	uint8_t x;
	uint8_t y;
	uint8_t s;
	uint16_t pc;
	uint8_t p;
} cpu_regs_t;

typedef struct cpu
{
	cpu_regs_t regs;
	mem_t *mem;
	uint8_t clock_count;
	uint8_t instr_delay;
} cpu_t;

cpu_t *cpu_new(mem_t *mem);
void cpu_del(cpu_t *cpu);
void cpu_clock(cpu_t *cpu);

#endif
