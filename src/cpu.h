#ifndef CPU_H
#define CPU_H

#include <stdint.h>

typedef struct mem mem_t;

enum cpu_flag
{
	CPU_FLAG_C = (1 << 0),
	CPU_FLAG_Z = (1 << 1),
	CPU_FLAG_I = (1 << 2),
	CPU_FLAG_D = (1 << 3),
	CPU_FLAG_B = (1 << 4),
	CPU_FLAG_V = (1 << 6),
	CPU_FLAG_N = (1 << 7),
};

#define CPU_GET_FLAG(cpu, f) (((cpu)->regs.p & (f)) ? 1 : 0)
#define CPU_GET_FLAG_C(cpu) CPU_GET_FLAG(cpu, CPU_FLAG_C)
#define CPU_GET_FLAG_Z(cpu) CPU_GET_FLAG(cpu, CPU_FLAG_Z)
#define CPU_GET_FLAG_I(cpu) CPU_GET_FLAG(cpu, CPU_FLAG_I)
#define CPU_GET_FLAG_D(cpu) CPU_GET_FLAG(cpu, CPU_FLAG_D)
#define CPU_GET_FLAG_B(cpu) CPU_GET_FLAG(cpu, CPU_FLAG_B)
#define CPU_GET_FLAG_V(cpu) CPU_GET_FLAG(cpu, CPU_FLAG_V)
#define CPU_GET_FLAG_N(cpu) CPU_GET_FLAG(cpu, CPU_FLAG_N)

#define CPU_SET_FLAG(cpu, f, v) \
do \
{ \
	if (v) \
		(cpu)->regs.p |= f; \
	else \
		(cpu)->regs.p &= ~f; \
} while (0)

#define CPU_SET_FLAG_C(cpu, v) CPU_SET_FLAG(cpu, CPU_FLAG_C, v)
#define CPU_SET_FLAG_Z(cpu, v) CPU_SET_FLAG(cpu, CPU_FLAG_Z, v)
#define CPU_SET_FLAG_I(cpu, v) CPU_SET_FLAG(cpu, CPU_FLAG_I, v)
#define CPU_SET_FLAG_D(cpu, v) CPU_SET_FLAG(cpu, CPU_FLAG_D, v)
#define CPU_SET_FLAG_B(cpu, v) CPU_SET_FLAG(cpu, CPU_FLAG_B, v)
#define CPU_SET_FLAG_V(cpu, v) CPU_SET_FLAG(cpu, CPU_FLAG_V, v)
#define CPU_SET_FLAG_N(cpu, v) CPU_SET_FLAG(cpu, CPU_FLAG_N, v)

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

uint8_t cpu_peek8(cpu_t *cpu);
uint16_t cpu_peek16(cpu_t *cpu);
uint8_t cpu_fetch8(cpu_t *cpu);
uint16_t cpu_fetch16(cpu_t *cpu);

#endif
