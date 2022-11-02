#include "instr.h"
#include "../cpu.h"
#include "../mem.h"
#include <inttypes.h>
#include <string.h>
#include <stdio.h>

#define CPU_INSTR(name) \
static const cpu_instr_t instr_##name = \
{ \
	.exec = exec_##name, \
	.print = print_##name, \
}

static void exec_clc(cpu_t *cpu)
{
	CPU_SET_FLAG_C(cpu, 0);
}

static void print_clc(cpu_t *cpu, char *data, size_t size)
{
	(void)cpu;
	snprintf(data, size, "clc");
}

CPU_INSTR(clc);

static void exec_sec(cpu_t *cpu)
{
	CPU_SET_FLAG_C(cpu, 1);
}

static void print_sec(cpu_t *cpu, char *data, size_t size)
{
	(void)cpu;
	snprintf(data, size, "sec");
}

CPU_INSTR(sec);

static void exec_cli(cpu_t *cpu)
{
	CPU_SET_FLAG_I(cpu, 0);
}

static void print_cli(cpu_t *cpu, char *data, size_t size)
{
	(void)cpu;
	snprintf(data, size, "cli");
}

CPU_INSTR(cli);

static void exec_sei(cpu_t *cpu)
{
	CPU_SET_FLAG_I(cpu, 1);
}

static void print_sei(cpu_t *cpu, char *data, size_t size)
{
	(void)cpu;
	snprintf(data, size, "sei");
}

CPU_INSTR(sei);

static void exec_clv(cpu_t *cpu)
{
	CPU_SET_FLAG_V(cpu, 0);
}

static void print_clv(cpu_t *cpu, char *data, size_t size)
{
	(void)cpu;
	snprintf(data, size, "clv");
}

CPU_INSTR(clv);

static void exec_cld(cpu_t *cpu)
{
	CPU_SET_FLAG_D(cpu, 0);
}

static void print_cld(cpu_t *cpu, char *data, size_t size)
{
	(void)cpu;
	snprintf(data, size, "cld");
}

CPU_INSTR(cld);

static void exec_sed(cpu_t *cpu)
{
	CPU_SET_FLAG_D(cpu, 1);
}

static void print_sed(cpu_t *cpu, char *data, size_t size)
{
	(void)cpu;
	snprintf(data, size, "sed");
}

CPU_INSTR(sed);

#define LDR_IMM(r) \
static void exec_ld##r##_imm(cpu_t *cpu) \
{ \
	uint8_t imm = cpu_fetch8(cpu); \
	cpu->regs.r = imm; \
	CPU_SET_FLAG_Z(cpu, !imm); \
	CPU_SET_FLAG_N(cpu, imm & 0x80); \
} \
static void print_ld##r##_imm(cpu_t *cpu, char *data, size_t size) \
{ \
	uint8_t imm = cpu_peek8(cpu); \
	snprintf(data, size, "ld" #r " #$%02" PRIx8, imm); \
} \
CPU_INSTR(ld##r##_imm)

LDR_IMM(a);
LDR_IMM(x);
LDR_IMM(y);

#define STR_IND16(r) \
static void exec_st##r##_ind16(cpu_t *cpu) \
{ \
	uint16_t ind = cpu_fetch16(cpu); \
	mem_set(cpu->mem, ind, cpu->regs.r); \
} \
static void print_st##r##_ind16(cpu_t *cpu, char *data, size_t size) \
{ \
	uint16_t ind = cpu_peek16(cpu); \
	snprintf(data, size, "st" #r " $%04" PRIx16, ind); \
} \
CPU_INSTR(st##r##_ind16)

STR_IND16(a);
STR_IND16(x);
STR_IND16(y);

#define TRR(rs, rd) \
static void exec_t##rs##rd(cpu_t *cpu) \
{ \
	cpu->regs.rd = cpu->regs.rs; \
	if (strcmp(#rs, "x") || strcmp(#rd, "s")) \
	{ \
		CPU_SET_FLAG_Z(cpu, !cpu->regs.rd); \
		CPU_SET_FLAG_N(cpu, cpu->regs.rd & 0x80); \
	} \
} \
static void print_t##rs##rd(cpu_t *cpu, char *data, size_t size) \
{ \
	(void)cpu; \
	snprintf(data, size, "t" #rs #rd); \
} \
CPU_INSTR(t##rs##rd)

TRR(a, y);
TRR(a, x);
TRR(s, x);
TRR(y, a);
TRR(x, a);
TRR(x, s);

#define STR_IND8(r) \
static void exec_st##r##_ind8(cpu_t *cpu) \
{ \
	uint8_t ind = cpu_fetch8(cpu); \
	mem_set(cpu->mem, ind, cpu->regs.r); \
} \
static void print_st##r##_ind8(cpu_t *cpu, char *data, size_t size) \
{ \
	uint8_t ind = cpu_peek8(cpu); \
	snprintf(data, size, "st" #r " $%02" PRIx8, ind); \
} \
CPU_INSTR(st##r##_ind8)

STR_IND8(a);
STR_IND8(x);
STR_IND8(y);

#define LDR_IND16_R(rd, rs) \
static void exec_ld##rd##_ind16_##rs(cpu_t *cpu) \
{ \
	uint16_t ind = cpu_fetch16(cpu); \
	cpu->regs.rd = mem_get(cpu->mem, ind + cpu->regs.rs); \
	CPU_SET_FLAG_Z(cpu, !cpu->regs.rd); \
	CPU_SET_FLAG_N(cpu, cpu->regs.rd & 0x80); \
} \
static void print_ld##rd##_ind16_##rs(cpu_t *cpu, char *data, size_t size) \
{ \
	uint16_t ind = cpu_peek16(cpu); \
	snprintf(data, size, "ld" #rd " $%04" PRIx16 ", " #rs, ind); \
} \
CPU_INSTR(ld##rd##_ind16_##rs)

LDR_IND16_R(a, x);
LDR_IND16_R(a, y);
LDR_IND16_R(x, y);
LDR_IND16_R(y, x);

#define LDR_IND16(r) \
static void exec_ld##r##_ind16(cpu_t *cpu) \
{ \
	uint16_t ind = cpu_fetch16(cpu); \
	cpu->regs.r = mem_get(cpu->mem, ind); \
	CPU_SET_FLAG_Z(cpu, !cpu->regs.r); \
	CPU_SET_FLAG_N(cpu, cpu->regs.r & 0x80); \
} \
static void print_ld##r##_ind16(cpu_t *cpu, char *data, size_t size) \
{ \
	uint16_t ind = cpu_peek16(cpu); \
	snprintf(data, size, "ld" #r " $%04" PRIx16, ind); \
} \
CPU_INSTR(ld##r##_ind16)

LDR_IND16(a);
LDR_IND16(x);
LDR_IND16(y);

#define LDR_IND8_R(rd, rs) \
static void exec_ld##rd##_ind8_##rs(cpu_t *cpu) \
{ \
	uint8_t ind = cpu_fetch8(cpu); \
	cpu->regs.rd = mem_get(cpu->mem, ind + cpu->regs.rs); \
	CPU_SET_FLAG_Z(cpu, !cpu->regs.rd); \
	CPU_SET_FLAG_N(cpu, cpu->regs.rd & 0x80); \
} \
static void print_ld##rd##_ind8_##rs(cpu_t *cpu, char *data, size_t size) \
{ \
	uint8_t ind = cpu_peek8(cpu); \
	snprintf(data, size, "ld" #rd " $%02" PRIx8 ", " #rs, ind); \
} \
CPU_INSTR(ld##rd##_ind8_##rs)

LDR_IND8_R(a, x);
LDR_IND8_R(x, y);
LDR_IND8_R(y, x);

#define BRANCH(name, flag, v) \
static void exec_##name(cpu_t *cpu) \
{ \
	int8_t dd = cpu_fetch8(cpu); \
	if (CPU_GET_FLAG(cpu, CPU_FLAG_##flag) == v) \
		cpu->regs.pc += dd; \
} \
static void print_##name(cpu_t *cpu, char *data, size_t size) \
{ \
	int8_t dd = cpu_peek8(cpu); \
	snprintf(data, size, #name " $%" PRId8, dd); \
} \
CPU_INSTR(name)

BRANCH(bpl, N, 0);
BRANCH(bmi, N, 1);
BRANCH(bvc, V, 0);
BRANCH(bvs, V, 1);
BRANCH(bcc, C, 0);
BRANCH(bcs, C, 1);
BRANCH(bne, Z, 0);
BRANCH(beq, Z, 1);

static void exec_pha(cpu_t *cpu)
{
	mem_set(cpu->mem, 0x100 + cpu->regs.s--, cpu->regs.a);
}

static void print_pha(cpu_t *cpu, char *data, size_t size)
{
	(void)cpu;
	snprintf(data, size, "pha");
}

CPU_INSTR(pha);

static void exec_php(cpu_t *cpu)
{
	mem_set(cpu->mem, 0x100 + cpu->regs.s--, cpu->regs.p);
}

static void print_php(cpu_t *cpu, char *data, size_t size)
{
	(void)cpu;
	snprintf(data, size, "php");
}

CPU_INSTR(php);

static void exec_pla(cpu_t *cpu)
{
	cpu->regs.a = mem_get(cpu->mem, 0x100 + ++cpu->regs.s);
	CPU_SET_FLAG_Z(cpu, !cpu->regs.a);
	CPU_SET_FLAG_N(cpu, cpu->regs.a & 0x80);
}

static void print_pla(cpu_t *cpu, char *data, size_t size)
{
	(void)cpu;
	snprintf(data, size, "pla");
}

CPU_INSTR(pla);

static void exec_plp(cpu_t *cpu)
{
	cpu->regs.p = mem_get(cpu->mem, 0x100 + ++cpu->regs.s);
}

static void print_plp(cpu_t *cpu, char *data, size_t size)
{
	(void)cpu;
	snprintf(data, size, "plp");
}

CPU_INSTR(plp);

static void exec_jsr(cpu_t *cpu)
{
	uint16_t imm = cpu_fetch16(cpu);
	uint16_t pc = cpu->regs.pc - 1;
	mem_set(cpu->mem, 0x100 + cpu->regs.s--, pc >> 8);
	mem_set(cpu->mem, 0x100 + cpu->regs.s--, pc >> 0);
	cpu->regs.pc = imm;
}

static void print_jsr(cpu_t *cpu, char *data, size_t size)
{
	uint16_t imm = cpu_peek16(cpu);
	snprintf(data, size, "jsr $%" PRIx16, imm);
}

CPU_INSTR(jsr);

static void exec_rti(cpu_t *cpu)
{
	cpu->regs.p = (cpu->regs.p & 0x30)
	            | (mem_get(cpu->mem, 0x100 + ++cpu->regs.s) & 0xCF);
	uint16_t lo = mem_get(cpu->mem, 0x100 + ++cpu->regs.s);
	uint16_t hi = mem_get(cpu->mem, 0x100 + ++cpu->regs.s);
	cpu->regs.pc = lo | (hi << 8);
}

static void print_rti(cpu_t *cpu, char *data, size_t size)
{
	(void)cpu;
	snprintf(data, size, "rti");
}

CPU_INSTR(rti);

static void exec_rts(cpu_t *cpu)
{
	uint16_t lo = mem_get(cpu->mem, 0x100 + ++cpu->regs.s);
	uint16_t hi = mem_get(cpu->mem, 0x100 + ++cpu->regs.s);
	cpu->regs.pc = (lo | (hi << 8)) + 1;
}

static void print_rts(cpu_t *cpu, char *data, size_t size)
{
	(void)cpu;
	snprintf(data, size, "rts");
}

CPU_INSTR(rts);

static void exec_jmp_imm(cpu_t *cpu)
{
	uint16_t imm = cpu_fetch16(cpu);
	cpu->regs.pc = imm;
}

static void print_jmp_imm(cpu_t *cpu, char *data, size_t size)
{
	uint16_t imm = cpu_peek16(cpu);
	snprintf(data, size, "jmp ($%04" PRIx16 ")", imm);
}

CPU_INSTR(jmp_imm);

static void exec_jmp_ind(cpu_t *cpu)
{
	uint16_t ind = cpu_fetch16(cpu);
	uint16_t lo = mem_get(cpu->mem, ind + 0);
	uint16_t hi = mem_get(cpu->mem, ind + 1);
	cpu->regs.pc = lo | (hi << 8);
}

static void print_jmp_ind(cpu_t *cpu, char *data, size_t size)
{
	uint16_t imm = cpu_peek16(cpu);
	snprintf(data, size, "jmp ($%04" PRIx16 ")", imm);
}

CPU_INSTR(jmp_ind);

static void exec_brk(cpu_t *cpu)
{
	CPU_SET_FLAG_B(cpu, 1);
	mem_set(cpu->mem, 0x100 + cpu->regs.s--, cpu->regs.pc >> 8);
	mem_set(cpu->mem, 0x100 + cpu->regs.s--, cpu->regs.pc >> 0);
	mem_set(cpu->mem, 0x100 + cpu->regs.s--, cpu->regs.p);
	CPU_SET_FLAG_I(cpu, 1);
	cpu->regs.pc = 0xFFFE;
}

static void print_brk(cpu_t *cpu, char *data, size_t size)
{
	(void)cpu;
	snprintf(data, size, "brk");
}

CPU_INSTR(brk);

static void exec_nop(cpu_t *cpu)
{
	(void)cpu;
}

static void print_nop(cpu_t *cpu, char *data, size_t size)
{
	(void)cpu;
	snprintf(data, size, "nop");
}

CPU_INSTR(nop);

static void exec_lda_ind_x(cpu_t *cpu)
{
	uint8_t imm = cpu_fetch8(cpu);
	cpu->regs.a = mem_get(cpu->mem, mem_get(cpu->mem, imm + cpu->regs.x));
}

static void print_lda_ind_x(cpu_t *cpu, char *data, size_t size)
{
	uint8_t imm = cpu_peek8(cpu);
	snprintf(data, size, "lda ($%02" PRIx8 ", x)", imm);
}

CPU_INSTR(lda_ind_x);

static void exec_lda_ind_y(cpu_t *cpu)
{
	uint8_t imm = cpu_fetch8(cpu);
	cpu->regs.a = mem_get(cpu->mem, mem_get(cpu->mem, imm) + cpu->regs.x);
}

static void print_lda_ind_y(cpu_t *cpu, char *data, size_t size)
{
	uint8_t imm = cpu_peek8(cpu);
	snprintf(data, size, "lda ($%02" PRIx8 "), y", imm);
}

CPU_INSTR(lda_ind_y);

#define STA_IND16_R(r) \
static void exec_sta_ind16_##r(cpu_t *cpu) \
{ \
	uint16_t ind = cpu_fetch16(cpu); \
	mem_set(cpu->mem, ind + cpu->regs.r, cpu->regs.a); \
} \
static void print_sta_ind16_##r(cpu_t *cpu, char *data, size_t size) \
{ \
	uint16_t ind = cpu_peek16(cpu); \
	snprintf(data, size, "sta $%04" PRIx16 ", " #r, ind); \
} \
CPU_INSTR(sta_ind16_##r)

STA_IND16_R(x);
STA_IND16_R(y);

#define STR_IND8_R(rd, rs) \
static void exec_st##rd##_ind8_##rs(cpu_t *cpu) \
{ \
	uint8_t ind8 = cpu_fetch8(cpu); \
	mem_set(cpu->mem, (uint16_t)ind8 + cpu->regs.rs, cpu->regs.rd); \
} \
static void print_st##rd##_ind8_##rs(cpu_t *cpu, char *data, size_t size) \
{ \
	uint8_t ind8 = cpu_peek8(cpu); \
	snprintf(data, size, "st" #rd " $%02" PRIx8 ", " #rs, ind8); \
} \
CPU_INSTR(st##rd##_ind8_##rs)

STR_IND8_R(a, x);
STR_IND8_R(x, y);
STR_IND8_R(y, x);

static void exec_sta_ind_x(cpu_t *cpu)
{
	uint8_t ind = cpu_fetch8(cpu);
	uint8_t dst8 = mem_get(cpu->mem, (uint16_t)ind + cpu->regs.x);
	uint16_t dst16 = ((uint16_t)mem_get(cpu->mem, dst8 + 0) << 0)
	               | ((uint16_t)mem_get(cpu->mem, dst8 + 1) << 8);
	mem_set(cpu->mem, dst16, cpu->regs.a);
}

static void print_sta_ind_x(cpu_t *cpu, char *data, size_t size)
{
	uint8_t ind = cpu_peek8(cpu);
	snprintf(data, size, "sta ($%02" PRIx8 ", x)", ind);
}

CPU_INSTR(sta_ind_x);

static void exec_sta_ind_y(cpu_t *cpu)
{
	uint8_t ind = cpu_fetch8(cpu);
	uint8_t dst8 = mem_get(cpu->mem, ind);
	uint16_t dst16 = ((uint16_t)mem_get(cpu->mem, dst8 + 0) << 0)
	               | ((uint16_t)mem_get(cpu->mem, dst8 + 1) << 8);
	dst16 += cpu->regs.y;
	mem_set(cpu->mem, dst16, cpu->regs.a);
}

static void print_sta_ind_y(cpu_t *cpu, char *data, size_t size)
{
	uint8_t ind = cpu_peek8(cpu);
	snprintf(data, size, "sta ($%02" PRIx8 "), y", ind);
}

CPU_INSTR(sta_ind_y);

#define INR(r) \
static void exec_in##r(cpu_t *cpu) \
{ \
	cpu->regs.r++; \
	CPU_SET_FLAG_Z(cpu, !cpu->regs.r); \
	CPU_SET_FLAG_N(cpu, cpu->regs.r & 0x80); \
} \
static void print_in##r(cpu_t *cpu, char *data, size_t size) \
{ \
	(void)cpu; \
	snprintf(data, size, "in" #r); \
} \
CPU_INSTR(in##r)

INR(x);
INR(y);

#define DER(r) \
static void exec_de##r(cpu_t *cpu) \
{ \
	cpu->regs.r--; \
	CPU_SET_FLAG_Z(cpu, !cpu->regs.r); \
	CPU_SET_FLAG_N(cpu, cpu->regs.r & 0x80); \
} \
static void print_de##r(cpu_t *cpu, char *data, size_t size) \
{ \
	(void)cpu; \
	snprintf(data, size, "de" #r); \
} \
CPU_INSTR(de##r)

DER(x);
DER(y);

#define INDE(name, op) \
static void exec_##name##c_ind8(cpu_t *cpu) \
{ \
	uint8_t ind = cpu_fetch8(cpu); \
	uint8_t val = mem_get(cpu->mem, ind) op 1; \
	mem_set(cpu->mem, ind, val); \
	CPU_SET_FLAG_Z(cpu, !val); \
	CPU_SET_FLAG_N(cpu, val & 0x80); \
} \
static void print_##name##c_ind8(cpu_t *cpu, char *data, size_t size) \
{ \
	uint8_t ind = cpu_peek8(cpu); \
	snprintf(data, size, #name "c $%02" PRIx8, ind); \
} \
CPU_INSTR(name##c_ind8); \
static void exec_##name##c_ind8_x(cpu_t *cpu) \
{ \
	uint8_t ind = cpu_fetch8(cpu); \
	uint8_t val = mem_get(cpu->mem, ind + cpu->regs.x) op 1; \
	mem_set(cpu->mem, ind + cpu->regs.x, val); \
	CPU_SET_FLAG_Z(cpu, !val); \
	CPU_SET_FLAG_N(cpu, val & 0x80); \
} \
static void print_##name##c_ind8_x(cpu_t *cpu, char *data, size_t size) \
{ \
	uint8_t ind = cpu_peek8(cpu); \
	snprintf(data, size, #name "c $%02" PRIx8 ", x", ind); \
} \
CPU_INSTR(name##c_ind8_x); \
static void exec_##name##c_ind16(cpu_t *cpu) \
{ \
	uint16_t ind = cpu_fetch16(cpu); \
	uint8_t val = mem_get(cpu->mem, ind) op 1; \
	mem_set(cpu->mem, ind, val); \
	CPU_SET_FLAG_Z(cpu, !val); \
	CPU_SET_FLAG_N(cpu, val & 0x80); \
} \
static void print_##name##c_ind16(cpu_t *cpu, char *data, size_t size) \
{ \
	uint16_t ind = cpu_peek16(cpu); \
	snprintf(data, size, #name "c $%04" PRIx16, ind); \
} \
CPU_INSTR(name##c_ind16); \
static void exec_##name##c_ind16_x(cpu_t *cpu) \
{ \
	uint16_t ind = cpu_fetch16(cpu); \
	uint8_t val = mem_get(cpu->mem, ind + cpu->regs.x) op 1; \
	mem_set(cpu->mem, ind + cpu->regs.x, val); \
	CPU_SET_FLAG_Z(cpu, !val); \
	CPU_SET_FLAG_N(cpu, val & 0x80); \
} \
static void print_##name##c_ind16_x(cpu_t *cpu, char *data, size_t size) \
{ \
	uint16_t ind = cpu_peek16(cpu); \
	snprintf(data, size, #name "c $%04" PRIx16 ", x", ind); \
} \
CPU_INSTR(name##c_ind16_x)

INDE(in, +);
INDE(de, -);

static uint8_t asl(cpu_t *cpu, uint8_t v)
{
	CPU_SET_FLAG_C(cpu, v & 0x80);
	return v << 1;
}

static uint8_t lsr(cpu_t *cpu, uint8_t v)
{
	CPU_SET_FLAG_C(cpu, v & 1);
	return v >> 1;
}

static uint8_t rol(cpu_t *cpu, uint8_t v)
{
	uint8_t c = v >> 7;
	v = (v << 1) | CPU_GET_FLAG_C(cpu);
	CPU_SET_FLAG_C(cpu, c);
	return v;
}

static uint8_t ror(cpu_t *cpu, uint8_t v)
{
	uint8_t c = v & 1;
	v = (v >> 1) | (CPU_GET_FLAG_C(cpu) << 7);
	CPU_SET_FLAG_C(cpu, c);
	return v;
}

#define SH_ROT(op) \
static void exec_##op##_a(cpu_t *cpu) \
{ \
	cpu->regs.a = op(cpu, cpu->regs.a); \
	CPU_SET_FLAG_Z(cpu, !cpu->regs.a); \
	CPU_SET_FLAG_N(cpu, cpu->regs.a & 0x80); \
} \
static void print_##op##_a(cpu_t *cpu, char *data, size_t size) \
{ \
	(void)cpu; \
	snprintf(data, size, #op " a"); \
} \
CPU_INSTR(op##_a); \
static void exec_##op##_ind8(cpu_t *cpu) \
{ \
	uint8_t ind = cpu_fetch8(cpu); \
	uint8_t v = op(cpu, mem_get(cpu->mem, ind)); \
	mem_set(cpu->mem, ind, v); \
	CPU_SET_FLAG_Z(cpu, !v); \
	CPU_SET_FLAG_N(cpu, v & 0x80); \
} \
static void print_##op##_ind8(cpu_t *cpu, char *data, size_t size) \
{ \
	uint8_t ind = cpu_peek8(cpu); \
	snprintf(data, size, #op " $%02" PRIx8, ind); \
} \
CPU_INSTR(op##_ind8); \
static void exec_##op##_ind8_x(cpu_t *cpu) \
{ \
	uint8_t ind = cpu_fetch8(cpu); \
	uint8_t v = op(cpu, mem_get(cpu->mem, ind + cpu->regs.x)); \
	mem_set(cpu->mem, ind + cpu->regs.x, v); \
	CPU_SET_FLAG_Z(cpu, !v); \
	CPU_SET_FLAG_N(cpu, v & 0x80); \
} \
static void print_##op##_ind8_x(cpu_t *cpu, char *data, size_t size) \
{ \
	uint8_t ind = cpu_peek8(cpu); \
	snprintf(data, size, #op " $%02" PRIx8 ", x", ind); \
} \
CPU_INSTR(op##_ind8_x); \
static void exec_##op##_ind16(cpu_t *cpu) \
{ \
	uint16_t ind = cpu_fetch16(cpu); \
	uint8_t v = op(cpu, mem_get(cpu->mem, ind)); \
	mem_set(cpu->mem, ind, v); \
	CPU_SET_FLAG_Z(cpu, !v); \
	CPU_SET_FLAG_N(cpu, v & 0x80); \
} \
static void print_##op##_ind16(cpu_t *cpu, char *data, size_t size) \
{ \
	uint8_t ind = cpu_peek16(cpu); \
	snprintf(data, size, #op " $%04" PRIx16, ind); \
} \
CPU_INSTR(op##_ind16); \
static void exec_##op##_ind16_x(cpu_t *cpu) \
{ \
	uint16_t ind = cpu_fetch16(cpu); \
	uint8_t v = op(cpu, mem_get(cpu->mem, ind + cpu->regs.x)); \
	mem_set(cpu->mem, ind + cpu->regs.x, v); \
	CPU_SET_FLAG_Z(cpu, !v); \
	CPU_SET_FLAG_N(cpu, v & 0x80); \
} \
static void print_##op##_ind16_x(cpu_t *cpu, char *data, size_t size) \
{ \
	uint8_t ind = cpu_peek16(cpu); \
	snprintf(data, size, #op " $%04" PRIx16 ", x", ind); \
} \
CPU_INSTR(op##_ind16_x)

SH_ROT(asl);
SH_ROT(lsr);
SH_ROT(rol);
SH_ROT(ror);

#define CPR(name, r) \
static void exec_##name##_imm(cpu_t *cpu) \
{ \
	uint8_t imm = cpu_fetch8(cpu); \
	uint8_t v = cpu->regs.r - imm; \
	CPU_SET_FLAG_C(cpu, v > cpu->regs.r); \
	CPU_SET_FLAG_Z(cpu, !v); \
	CPU_SET_FLAG_N(cpu, v & 0x80); \
} \
static void print_##name##_imm(cpu_t *cpu, char *data, size_t size) \
{ \
	uint8_t imm = cpu_peek8(cpu); \
	snprintf(data, size, #name " #$%02" PRIx8, imm); \
} \
CPU_INSTR(name##_imm); \
static void exec_##name##_ind8(cpu_t *cpu) \
{ \
	uint8_t ind = cpu_fetch8(cpu); \
	uint8_t v = cpu->regs.r - mem_get(cpu->mem, ind); \
	CPU_SET_FLAG_C(cpu, v > cpu->regs.r); \
	CPU_SET_FLAG_Z(cpu, !v); \
	CPU_SET_FLAG_N(cpu, v & 0x80); \
} \
static void print_##name##_ind8(cpu_t *cpu, char *data, size_t size) \
{ \
	uint8_t ind = cpu_peek8(cpu); \
	snprintf(data, size, #name " $%02" PRIx8, ind); \
} \
CPU_INSTR(name##_ind8); \
static void exec_##name##_ind16(cpu_t *cpu) \
{ \
	uint16_t ind = cpu_fetch16(cpu); \
	uint8_t v = cpu->regs.r - mem_get(cpu->mem, ind); \
	CPU_SET_FLAG_C(cpu, v > cpu->regs.r); \
	CPU_SET_FLAG_Z(cpu, !v); \
	CPU_SET_FLAG_N(cpu, v & 0x80); \
} \
static void print_##name##_ind16(cpu_t *cpu, char *data, size_t size) \
{ \
	uint16_t ind = cpu_peek16(cpu); \
	snprintf(data, size, #name " $%04" PRIx16, ind); \
} \
CPU_INSTR(name##_ind16)

CPR(cmp, a);
CPR(cpx, x);
CPR(cpy, y);

#define CMP_IND16_R(r) \
static void exec_cmp_ind16_##r(cpu_t *cpu) \
{ \
	uint16_t ind = cpu_fetch16(cpu); \
	uint8_t v = cpu->regs.a - mem_get(cpu->mem, ind + cpu->regs.r); \
	CPU_SET_FLAG_C(cpu, v > cpu->regs.a); \
	CPU_SET_FLAG_Z(cpu, !v); \
	CPU_SET_FLAG_N(cpu, v & 0x80); \
} \
static void print_cmp_ind16_##r(cpu_t *cpu, char *data, size_t size) \
{ \
	uint16_t ind = cpu_peek16(cpu); \
	snprintf(data, size, "cmp $%04" PRIx16, ind); \
} \
CPU_INSTR(cmp_ind16_##r)

CMP_IND16_R(x);
CMP_IND16_R(y);

static void exec_cmp_ind8_x(cpu_t *cpu)
{
	uint8_t ind = cpu_fetch8(cpu);
	uint8_t v = cpu->regs.a - mem_get(cpu->mem, ind + cpu->regs.x);
	CPU_SET_FLAG_C(cpu, v > cpu->regs.a);
	CPU_SET_FLAG_Z(cpu, !v);
	CPU_SET_FLAG_N(cpu, v & 0x80);
}

static void print_cmp_ind8_x(cpu_t *cpu, char *data, size_t size)
{
	uint8_t ind = cpu_peek8(cpu);
	snprintf(data, size, "cmp $%02" PRIx8 ", x", ind);
}

CPU_INSTR(cmp_ind8_x);

static void exec_cmp_ind_x(cpu_t *cpu)
{
	uint8_t ind = cpu_fetch8(cpu);
	uint8_t mem8 = mem_get(cpu->mem, ind + cpu->regs.x);
	uint16_t mem16 = ((uint16_t)mem_get(cpu->mem, mem8 + 0) << 0)
	               | ((uint16_t)mem_get(cpu->mem, mem8 + 1) << 8);
	uint8_t v = cpu->regs.a - mem_get(cpu->mem, mem16);
	CPU_SET_FLAG_C(cpu, v > cpu->regs.a);
	CPU_SET_FLAG_Z(cpu, !v);
	CPU_SET_FLAG_N(cpu, v & 0x80);
}

static void print_cmp_ind_x(cpu_t *cpu, char *data, size_t size)
{
	uint8_t ind = cpu_peek8(cpu);
	snprintf(data, size, "cmp ($%02" PRIx8 ", x)", ind);
}

CPU_INSTR(cmp_ind_x);

static void exec_cmp_ind_y(cpu_t *cpu)
{
	uint8_t ind = cpu_fetch8(cpu);
	uint8_t mem8 = mem_get(cpu->mem, ind);
	uint16_t mem16 = ((uint16_t)mem_get(cpu->mem, mem8 + 0) << 0)
	               | ((uint16_t)mem_get(cpu->mem, mem8 + 1) << 8);
	mem16 += cpu->regs.y;
	uint8_t v = cpu->regs.a - mem_get(cpu->mem, mem16);
	CPU_SET_FLAG_C(cpu, v > cpu->regs.a);
	CPU_SET_FLAG_Z(cpu, !v);
	CPU_SET_FLAG_N(cpu, v & 0x80);
}

static void print_cmp_ind_y(cpu_t *cpu, char *data, size_t size)
{
	uint8_t ind = cpu_peek8(cpu);
	snprintf(data, size, "cmp ($%02" PRIx8 ", x)", ind);
}

CPU_INSTR(cmp_ind_y);

static void exec_bit_ind8(cpu_t *cpu)
{
	uint8_t ind = cpu_fetch8(cpu);
	uint8_t mem = mem_get(cpu->mem, ind);
	uint8_t v = cpu->regs.a & mem;
	CPU_SET_FLAG_Z(cpu, !v);
	CPU_SET_FLAG_N(cpu, mem & 0x80);
	CPU_SET_FLAG_V(cpu, mem & 0x40);
}

static void print_bit_ind8(cpu_t *cpu, char *data, size_t size)
{
	uint8_t ind = cpu_peek8(cpu);
	snprintf(data, size, "bit $%02" PRIx8, ind);
}

CPU_INSTR(bit_ind8);

static void exec_bit_ind16(cpu_t *cpu)
{
	uint16_t ind = cpu_fetch16(cpu);
	uint8_t mem = mem_get(cpu->mem, ind);
	uint8_t v = cpu->regs.a & mem;
	CPU_SET_FLAG_Z(cpu, !v);
	CPU_SET_FLAG_N(cpu, mem & 0x80);
	CPU_SET_FLAG_V(cpu, mem & 0x40);
}

static void print_bit_ind16(cpu_t *cpu, char *data, size_t size)
{
	uint16_t ind = cpu_peek16(cpu);
	snprintf(data, size, "bit $%04" PRIx16, ind);
}

CPU_INSTR(bit_ind16);

static void adc(cpu_t *cpu, uint8_t n)
{
	uint8_t op = n + CPU_GET_FLAG_C(cpu);
	uint8_t v = cpu->regs.a + op;
	CPU_SET_FLAG_N(cpu, v & 0x80);
	CPU_SET_FLAG_Z(cpu, !v);
	CPU_SET_FLAG_C(cpu, v < cpu->regs.a);
	CPU_SET_FLAG_V(cpu, (~(cpu->regs.a ^ op) & (v ^ op)) & 0x80);
	cpu->regs.a = v;
}

static void sbc(cpu_t *cpu, uint8_t n)
{
	uint8_t op = 1 + n - CPU_GET_FLAG_C(cpu);
	uint8_t v = cpu->regs.a - op;
	CPU_SET_FLAG_N(cpu, v & 0x80);
	CPU_SET_FLAG_Z(cpu, !v);
	CPU_SET_FLAG_C(cpu, op <= cpu->regs.a);
	CPU_SET_FLAG_V(cpu, ((cpu->regs.a ^ op) & (v ^ op)) & 0x80);
	cpu->regs.a = v;
}

static void and(cpu_t *cpu, uint8_t n)
{
	uint8_t v = cpu->regs.a & n;
	CPU_SET_FLAG_N(cpu, v & 0x80);
	CPU_SET_FLAG_Z(cpu, !v);
	cpu->regs.a = v;
}

static void eor(cpu_t *cpu, uint8_t n)
{
	uint8_t v = cpu->regs.a ^ n;
	CPU_SET_FLAG_N(cpu, v & 0x80);
	CPU_SET_FLAG_Z(cpu, !v);
	cpu->regs.a = v;
}

static void ora(cpu_t *cpu, uint8_t n)
{
	uint8_t v = cpu->regs.a | n;
	CPU_SET_FLAG_N(cpu, v & 0x80);
	CPU_SET_FLAG_Z(cpu, !v);
	cpu->regs.a = v;
}

#define ALU(op) \
static void exec_##op##_imm(cpu_t *cpu) \
{ \
	uint8_t imm = cpu_fetch8(cpu); \
	op(cpu, imm); \
} \
static void print_##op##_imm(cpu_t *cpu, char *data, size_t size) \
{ \
	uint8_t imm = cpu_peek8(cpu); \
	snprintf(data, size, #op " #$%02" PRIx8, imm); \
} \
CPU_INSTR(op##_imm); \
static void exec_##op##_ind8(cpu_t *cpu) \
{ \
	uint8_t ind = cpu_fetch8(cpu); \
	op(cpu, mem_get(cpu->mem, ind)); \
} \
static void print_##op##_ind8(cpu_t *cpu, char *data, size_t size) \
{ \
	uint8_t ind = cpu_peek8(cpu); \
	snprintf(data, size, #op " $%02" PRIx8, ind); \
} \
CPU_INSTR(op##_ind8); \
static void exec_##op##_ind8_x(cpu_t *cpu) \
{ \
	uint8_t ind = cpu_fetch8(cpu); \
	op(cpu, mem_get(cpu->mem, ind + cpu->regs.x)); \
} \
static void print_##op##_ind8_x(cpu_t *cpu, char *data, size_t size) \
{ \
	uint8_t ind = cpu_peek8(cpu); \
	snprintf(data, size, #op " $%02" PRIx8 ", x", ind); \
} \
CPU_INSTR(op##_ind8_x); \
static void exec_##op##_ind16(cpu_t *cpu) \
{ \
	uint16_t ind = cpu_fetch16(cpu); \
	op(cpu, mem_get(cpu->mem, ind)); \
} \
static void print_##op##_ind16(cpu_t *cpu, char *data, size_t size) \
{ \
	uint16_t ind = cpu_peek16(cpu); \
	snprintf(data, size, #op " $%04" PRIx16, ind); \
} \
CPU_INSTR(op##_ind16); \
static void exec_##op##_ind16_x(cpu_t *cpu) \
{ \
	uint16_t ind = cpu_fetch16(cpu); \
	op(cpu, mem_get(cpu->mem, ind + cpu->regs.x)); \
} \
static void print_##op##_ind16_x(cpu_t *cpu, char *data, size_t size) \
{ \
	uint16_t ind = cpu_peek16(cpu); \
	snprintf(data, size, #op " $%04" PRIx16 ", x", ind); \
} \
CPU_INSTR(op##_ind16_x); \
static void exec_##op##_ind16_y(cpu_t *cpu) \
{ \
	uint16_t ind = cpu_fetch16(cpu); \
	op(cpu, mem_get(cpu->mem, ind + cpu->regs.y)); \
} \
static void print_##op##_ind16_y(cpu_t *cpu, char *data, size_t size) \
{ \
	uint16_t ind = cpu_peek16(cpu); \
	snprintf(data, size, #op " $%04" PRIx16 ", y", ind); \
} \
CPU_INSTR(op##_ind16_y); \
static void exec_##op##_ind_x(cpu_t *cpu) \
{ \
	uint8_t ind = cpu_fetch8(cpu); \
	uint8_t mem8 = mem_get(cpu->mem, ind + cpu->regs.x); \
	uint16_t mem16 = ((uint16_t)mem_get(cpu->mem, mem8 + 0) << 0) \
	               | ((uint16_t)mem_get(cpu->mem, mem8 + 1) << 8); \
	op(cpu, mem_get(cpu->mem, mem16)); \
} \
static void print_##op##_ind_x(cpu_t *cpu, char *data, size_t size) \
{ \
	uint8_t ind = cpu_peek8(cpu); \
	snprintf(data, size, #op "($%02" PRIx8 ", x)", ind); \
} \
CPU_INSTR(op##_ind_x); \
static void exec_##op##_ind_y(cpu_t *cpu) \
{ \
	uint8_t ind = cpu_fetch8(cpu); \
	uint8_t mem8 = mem_get(cpu->mem, ind); \
	uint16_t mem16 = ((uint16_t)mem_get(cpu->mem, mem8 + 0) << 0) \
	               | ((uint16_t)mem_get(cpu->mem, mem8 + 1) << 8); \
	mem16 += cpu->regs.y; \
	op(cpu, mem_get(cpu->mem, mem16)); \
} \
static void print_##op##_ind_y(cpu_t *cpu, char *data, size_t size) \
{ \
	uint8_t ind = cpu_peek8(cpu); \
	snprintf(data, size, #op "($%02" PRIx8 "), y", ind); \
} \
CPU_INSTR(op##_ind_y)

ALU(adc);
ALU(sbc);
ALU(and);
ALU(eor);
ALU(ora);

static void exec_irq(cpu_t *cpu)
{
	CPU_SET_FLAG_B(cpu, 0);
	uint16_t pc = cpu->regs.pc - 1;
	mem_set(cpu->mem, cpu->regs.s--, pc >> 8);
	mem_set(cpu->mem, cpu->regs.s--, pc >> 0);
	mem_set(cpu->mem, cpu->regs.s--, cpu->regs.p);
	CPU_SET_FLAG_I(cpu, 1);
	cpu->regs.pc = 0xFFFE;
}

static void print_irq(cpu_t *cpu, char *data, size_t size)
{
	(void)cpu;
	snprintf(data, size, "irq");
}

const cpu_instr_t irq_instr =
{
	.exec = exec_irq,
	.print = print_irq,
};

static void exec_nmi(cpu_t *cpu)
{
	CPU_SET_FLAG_B(cpu, 0);
	uint16_t pc = cpu->regs.pc - 1;
	mem_set(cpu->mem, cpu->regs.s--, pc >> 8);
	mem_set(cpu->mem, cpu->regs.s--, pc >> 0);
	mem_set(cpu->mem, cpu->regs.s--, cpu->regs.p);
	CPU_SET_FLAG_I(cpu, 1);
	cpu->regs.pc = 0xFFFA;
}

static void print_nmi(cpu_t *cpu, char *data, size_t size)
{
	(void)cpu;
	snprintf(data, size, "nmi");
}

const cpu_instr_t nmi_instr =
{
	.exec = exec_nmi,
	.print = print_nmi,
};

static void exec_reset(cpu_t *cpu)
{
	CPU_SET_FLAG_B(cpu, 1);
	cpu->regs.s -= 3;
	CPU_SET_FLAG_I(cpu, 1);
	cpu->regs.pc = 0xFFFC;
}

static void print_reset(cpu_t *cpu, char *data, size_t size)
{
	(void)cpu;
	snprintf(data, size, "reset");
}

const cpu_instr_t reset_instr =
{
	.exec = exec_reset,
	.print = print_reset,
};

const cpu_instr_t *cpu_instr[256] =
{
	[0x00] = &instr_brk,
	[0x01] = &instr_ora_ind_x,
	[0x05] = &instr_ora_ind8,
	[0x06] = &instr_asl_ind8,
	[0x08] = &instr_php,
	[0x09] = &instr_ora_imm,
	[0x0A] = &instr_asl_a,
	[0x0D] = &instr_ora_ind16,
	[0x0E] = &instr_asl_ind16,
	[0x10] = &instr_bpl,
	[0x11] = &instr_ora_ind_y,
	[0x15] = &instr_ora_ind8_x,
	[0x16] = &instr_asl_ind8_x,
	[0x18] = &instr_clc,
	[0x19] = &instr_ora_ind16_y,
	[0x1D] = &instr_ora_ind16_x,
	[0x1E] = &instr_asl_ind16_x,
	[0x20] = &instr_jsr,
	[0x21] = &instr_and_ind_x,
	[0x24] = &instr_bit_ind8,
	[0x25] = &instr_and_ind8,
	[0x26] = &instr_rol_ind8,
	[0x28] = &instr_plp,
	[0x29] = &instr_and_imm,
	[0x2A] = &instr_rol_a,
	[0x2C] = &instr_bit_ind16,
	[0x2D] = &instr_and_ind16,
	[0x2E] = &instr_rol_ind16,
	[0x30] = &instr_bmi,
	[0x31] = &instr_and_ind_y,
	[0x35] = &instr_and_ind8_x,
	[0x36] = &instr_rol_ind8_x,
	[0x38] = &instr_sec,
	[0x39] = &instr_and_ind16_y,
	[0x3D] = &instr_and_ind16_x,
	[0x3E] = &instr_rol_ind16_x,
	[0x40] = &instr_rti,
	[0x41] = &instr_eor_ind_x,
	[0x45] = &instr_eor_ind8,
	[0x46] = &instr_lsr_ind8,
	[0x48] = &instr_pha,
	[0x49] = &instr_eor_imm,
	[0x4A] = &instr_lsr_a,
	[0x4C] = &instr_jmp_imm,
	[0x4D] = &instr_eor_ind16,
	[0x4E] = &instr_lsr_ind16,
	[0x50] = &instr_bvc,
	[0x51] = &instr_eor_ind_y,
	[0x55] = &instr_eor_ind8_x,
	[0x56] = &instr_lsr_ind8_x,
	[0x58] = &instr_cli,
	[0x59] = &instr_eor_ind16_y,
	[0x5D] = &instr_eor_ind16_x,
	[0x5E] = &instr_lsr_ind16_x,
	[0x60] = &instr_rts,
	[0x61] = &instr_adc_ind_x,
	[0x65] = &instr_adc_ind8,
	[0x66] = &instr_ror_ind8,
	[0x68] = &instr_pla,
	[0x69] = &instr_adc_imm,
	[0x6A] = &instr_ror_a,
	[0x6C] = &instr_jmp_ind,
	[0x6D] = &instr_adc_ind16,
	[0x6E] = &instr_ror_ind16,
	[0x70] = &instr_bvs,
	[0x71] = &instr_adc_ind_y,
	[0x75] = &instr_adc_ind8_x,
	[0x76] = &instr_ror_ind8_x,
	[0x78] = &instr_sei,
	[0x79] = &instr_adc_ind16_y,
	[0x7D] = &instr_adc_ind16_x,
	[0x7E] = &instr_ror_ind16_x,
	[0x81] = &instr_sta_ind_x,
	[0x84] = &instr_sty_ind8,
	[0x85] = &instr_sta_ind8,
	[0x86] = &instr_stx_ind8,
	[0x88] = &instr_dey,
	[0x8A] = &instr_txa,
	[0x8C] = &instr_sty_ind16,
	[0x8D] = &instr_sta_ind16,
	[0x8E] = &instr_stx_ind16,
	[0x90] = &instr_bcc,
	[0x91] = &instr_sta_ind_y,
	[0x94] = &instr_sty_ind8_x,
	[0x95] = &instr_sta_ind8_x,
	[0x96] = &instr_stx_ind8_y,
	[0x98] = &instr_tya,
	[0x99] = &instr_sta_ind16_y,
	[0x9A] = &instr_txs,
	[0x9D] = &instr_sta_ind16_x,
	[0xA0] = &instr_ldy_imm,
	[0xA1] = &instr_lda_ind_x,
	[0xA2] = &instr_ldx_imm,
	[0xA8] = &instr_tay,
	[0xA9] = &instr_lda_imm,
	[0xAA] = &instr_tax,
	[0xAC] = &instr_ldy_ind16,
	[0xAD] = &instr_lda_ind16,
	[0xAE] = &instr_ldx_ind16,
	[0xB0] = &instr_bcs,
	[0xB1] = &instr_lda_ind_y,
	[0xB4] = &instr_ldy_ind8_x,
	[0xB5] = &instr_lda_ind8_x,
	[0xB6] = &instr_ldx_ind8_y,
	[0xB8] = &instr_clv,
	[0xB9] = &instr_lda_ind16_y,
	[0xBA] = &instr_tsx,
	[0xBC] = &instr_ldy_ind16_x,
	[0xBD] = &instr_lda_ind16_x,
	[0xBE] = &instr_ldx_ind16_y,
	[0xC0] = &instr_cpy_imm,
	[0xC1] = &instr_cmp_ind_x,
	[0xC4] = &instr_cpy_ind8,
	[0xC5] = &instr_cmp_ind8,
	[0xC6] = &instr_dec_ind8,
	[0xC8] = &instr_iny,
	[0xC9] = &instr_cmp_imm,
	[0xCA] = &instr_dex,
	[0xCC] = &instr_cpy_ind16,
	[0xCD] = &instr_cmp_ind16,
	[0xCE] = &instr_dec_ind16,
	[0xD0] = &instr_bne,
	[0xD1] = &instr_cmp_ind_y,
	[0xD5] = &instr_cmp_ind8_x,
	[0xD6] = &instr_dec_ind8_x,
	[0xD8] = &instr_cld,
	[0xD9] = &instr_cmp_ind16_y,
	[0xDD] = &instr_cmp_ind16_x,
	[0xDE] = &instr_dec_ind16_x,
	[0xE0] = &instr_cpx_imm,
	[0xE1] = &instr_sbc_ind_x,
	[0xE4] = &instr_cpx_ind8,
	[0xE5] = &instr_sbc_ind8,
	[0xE6] = &instr_inc_ind8,
	[0xE8] = &instr_inx,
	[0xE9] = &instr_sbc_imm,
	[0xEC] = &instr_cpx_ind16,
	[0xED] = &instr_sbc_ind16,
	[0xEE] = &instr_inc_ind16,
	[0xEA] = &instr_nop,
	[0xF0] = &instr_beq,
	[0xF1] = &instr_sbc_ind_y,
	[0xF5] = &instr_sbc_ind8_x,
	[0xF6] = &instr_inc_ind8_x,
	[0xF8] = &instr_sed,
	[0xF9] = &instr_sbc_ind16_y,
	[0xFD] = &instr_sbc_ind16_x,
	[0xFE] = &instr_inc_ind16_x,
};
