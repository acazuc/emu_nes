#include "cpu.h"
#include "cpu/instr.h"
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>

cpu_t *cpu_new(mem_t *mem)
{
	cpu_t *cpu = calloc(sizeof(*cpu), 1);
	if (!cpu)
		return NULL;
	cpu->mem = mem;
	return cpu;
}

void cpu_del(cpu_t *cpu)
{
	if (!cpu)
		return;
	free(cpu);
}

uint8_t cpu_fetch8(cpu_t *cpu)
{
	return mem_get(cpu->mem, cpu->regs.pc++);
}

static void cpu_cycle(cpu_t *cpu)
{
	if (cpu->instr_delay)
	{
		cpu->instr_delay--;
		return;
	}
	uint8_t opc = cpu_fetch8(cpu);
	const cpu_instr_t *instr = cpu_instr[opc];
	if (instr)
		instr->exec(cpu);
	else
		printf("unknown instruction %" PRIx8 "\n", opc);
}

void cpu_clock(cpu_t *cpu)
{
	if (++cpu->clock_count == 12) /* 16 in PAL */
	{
		cpu_cycle(cpu);
		cpu->clock_count = 0;
	}
}
