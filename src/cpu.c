#include "cpu.h"
#include "mem.h"
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
	cpu->regs.pc = 0x8000;
	return cpu;
}

void cpu_del(cpu_t *cpu)
{
	if (!cpu)
		return;
	free(cpu);
}

uint8_t cpu_peek8(cpu_t *cpu)
{
	return mem_get(cpu->mem, cpu->regs.pc);
}

uint16_t cpu_peek16(cpu_t *cpu)
{
	uint16_t lo = mem_get(cpu->mem, cpu->regs.pc + 0);
	uint16_t hi = mem_get(cpu->mem, cpu->regs.pc + 1);
	return lo | (hi << 8);
}

uint8_t cpu_fetch8(cpu_t *cpu)
{
	return mem_get(cpu->mem, cpu->regs.pc++);
}

uint16_t cpu_fetch16(cpu_t *cpu)
{
	uint16_t lo = cpu_fetch8(cpu);
	uint16_t hi = cpu_fetch8(cpu);
	return lo | (hi << 8);
}

static void cpu_cycle(cpu_t *cpu)
{
	if (cpu->instr_delay)
	{
		cpu->instr_delay--;
		return;
	}
	if (!CPU_GET_FLAG_I(cpu))
	{
		/* XXX IRQ */
	}
	/* XXX NMI */
	uint8_t opc = cpu_fetch8(cpu);
	const cpu_instr_t *instr = cpu_instr[opc];
	if (!instr)
	{
		printf("unknown instruction %" PRIx8 "\n", opc);
		return;
	}
	char tmp[256];
	instr->print(cpu, tmp, sizeof(tmp));
	printf("%-20s [OPC=%02" PRIx8 " A=%02" PRIx8 " X=%02" PRIx8 " Y=%02" PRIx8
	       " S=%02" PRIx8 " PC=%04" PRIx16 " P=%02" PRIx8 "]\n",
	       tmp, opc, cpu->regs.a, cpu->regs.x, cpu->regs.y, cpu->regs.s,
	       cpu->regs.pc, cpu->regs.p);
	instr->exec(cpu);
}

void cpu_clock(cpu_t *cpu)
{
	if (++cpu->clock_count == 12) /* 16 in PAL */
	{
		cpu_cycle(cpu);
		cpu->clock_count = 0;
	}
}
