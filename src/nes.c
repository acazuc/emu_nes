#include "nes.h"
#include "mbc.h"
#include "mem.h"
#include "apu.h"
#include "cpu.h"
#include "gpu.h"
#include <stdlib.h>
#include <string.h>

nes_t *nes_new(const void *rom_data, size_t rom_size)
{
	nes_t *nes = calloc(sizeof(*nes), 1);
	if (!nes)
		return NULL;

	nes->mbc = mbc_new(rom_data, rom_size);
	if (!nes->mbc)
		return NULL;

	nes->mem = mem_new(nes->mbc);
	if (!nes->mem)
		return NULL;

	nes->apu = apu_new(nes->mem);
	if (!nes->apu)
		return NULL;

	nes->cpu = cpu_new(nes->mem);
	if (!nes->cpu)
		return NULL;

	nes->gpu = gpu_new(nes->mem);
	if (!nes->gpu)
		return NULL;

	return nes;
}

void nes_del(nes_t *nes)
{
	if (!nes)
		return;
	gpu_del(nes->gpu);
	cpu_del(nes->cpu);
	apu_del(nes->apu);
	mem_del(nes->mem);
	mbc_del(nes->mbc);
	free(nes);
}

void nes_frame(nes_t *nes, uint8_t *video_buf, int16_t *audio_buf, uint32_t joypad)
{
	for (size_t i = 0 ; i < 357954; ++i) /* 532034 in PAL */
	{
		cpu_clock(nes->cpu);
		gpu_clock(nes->gpu);
	}
	memset(video_buf, 0, 256 * 240 * 4);
	memset(audio_buf, 0, 960 * 2);
}
