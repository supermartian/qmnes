/*
 * rom.c
 * Copyright (C) 2014 martian <supermartian@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "rom.h"

void load_rom(struct rom *rom, char *filename)
{
    rom->rom_fd = open(filename, O_RDWR);
    printf("file opened %d\n", rom->rom_fd);
    struct stat stats;
    fstat(rom->rom_fd, &stats);
    int filesize = stats.st_size;

    uint8_t *romfile = mmap(0, filesize, PROT_READ, MAP_SHARED, rom->rom_fd, 0);
    rom->rom_start = malloc(sizeof(uint8_t) * filesize);
    memcpy(rom->rom_start, romfile, sizeof(uint8_t) * filesize);
    printf("Rom loaded:");
    close(rom->rom_fd);

    uint8_t *p = rom->rom_start;
    rom->prg_rom_size = p[4];
    rom->chr_rom_size = p[5];
    rom->rom_mirroring = p[6] & 0x05;
    if (rom->rom_mirroring >> 3) {
        rom->rom_mirroring = 2;
    }

    printf("rom size %d, Prog size %d, CHR size %d\n", filesize, p[4], p[5]);

    rom->prg_rom_data = rom->rom_start + 16;
    rom->chr_rom_data = rom->prg_rom_data + 16384 * rom->prg_rom_size;
    printf("Prog size %x\n", rom->prg_rom_data);
    printf("Prog size %d\n", rom->chr_rom_data - rom->rom_start);
}

void unload_rom(struct rom *rom)
{
    close(rom->rom_fd);
}
