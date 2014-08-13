/*
 * rom.h
 * Copyright (C) 2014 martian <supermartian@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef __ROM_H__
#define __ROM_H__

#include <stdint.h>

struct rom {
    uint8_t prg_rom_size;
    uint8_t chr_rom_size;

    uint8_t *prg_rom_data;
    uint8_t *chr_rom_data;

    uint8_t *rom_start;
    uint8_t rom_mirroring;
    int rom_fd;
};

void load_rom(struct rom *rom, char *filename);

#endif /* !__ROM_H__ */

