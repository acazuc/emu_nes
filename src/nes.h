#ifndef NES_H
#define NES_H

#include <stddef.h>
#include <stdint.h>

typedef struct mbc mbc_t;
typedef struct mem mem_t;
typedef struct apu apu_t;
typedef struct cpu cpu_t;
typedef struct gpu gpu_t;

enum nes_button
{
	NES_BUTTON_RIGHT  = (1 << 0),
	NES_BUTTON_LEFT   = (1 << 1),
	NES_BUTTON_UP     = (1 << 2),
	NES_BUTTON_DOWN   = (1 << 3),
	NES_BUTTON_A      = (1 << 4),
	NES_BUTTON_B      = (1 << 5),
	NES_BUTTON_SELECT = (1 << 6),
	NES_BUTTON_START  = (1 << 7),
};

typedef struct nes
{
	mbc_t *mbc;
	mem_t *mem;
	apu_t *apu;
	cpu_t *cpu;
	gpu_t *gpu;
} nes_t;

nes_t *nes_new(const void *rom_data, size_t rom_size);
void nes_del(nes_t *nes);

void nes_frame(nes_t *nes, uint8_t *video_buf, int16_t *audio_buf, uint32_t joypad);

#endif
