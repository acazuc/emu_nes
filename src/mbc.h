#ifndef MBC_H
#define MBC_H

#include <stdint.h>
#include <stddef.h>

struct ines
{
	uint8_t magic[4];
	uint8_t prg_rom_lsb;
	uint8_t chr_rom_lsb;
	uint8_t flags6;
	uint8_t flags7;
	uint8_t mapper;
	uint8_t prg_chr_rom_msb;
	uint8_t eeprom_size;
	uint8_t chr_ram_size;
	uint8_t timing;
	uint8_t ext_type;
	uint8_t misc_roms;
	uint8_t ded;
};

typedef struct mbc
{
	uint8_t *data;
	size_t size;
	struct ines *ines;
	uint8_t *trainer;
	uint8_t *prg_rom_data;
	size_t prg_rom_size;
	uint8_t *chr_rom_data;
	size_t chr_rom_size;
} mbc_t;

mbc_t *mbc_new(const void *data, size_t size);
void mbc_del(mbc_t *mbc);

uint8_t mbc_get(mbc_t *mbc, uint16_t addr);
void mbc_set(mbc_t *mbc, uint16_t addr, uint8_t v);

uint8_t mbc_gpu_get(mbc_t *mbc, uint16_t addr);
void mbc_gpu_set(mbc_t *mbc, uint16_t addr, uint8_t v);

#endif
