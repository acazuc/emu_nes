#ifndef CPU_INSTR_H
#define CPU_INSTR_H

#include <stddef.h>

typedef struct cpu cpu_t;

typedef struct cpu_instr
{
	void (*exec)(cpu_t *cpu);
	void (*print)(cpu_t *cpu, char *data, size_t size);
} cpu_instr_t;

extern const cpu_instr_t *cpu_instr[256];

extern const cpu_instr_t irq_instr;
extern const cpu_instr_t nmi_instr;
extern const cpu_instr_t reset_instr;

#endif
