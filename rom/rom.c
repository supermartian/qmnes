/*
 * rom.c
 * Copyright (C) 2014 martian <supermartian@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */
#include <stdio.h>
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

    rom->rom_start = mmap(0, filesize, PROT_READ, MAP_SHARED, rom->rom_fd, 0);
    printf("Rom loaded:");

    uint8_t *p = rom->rom_start;
    rom->prg_rom_size = p[4];
    rom->chr_rom_size = p[8];

    printf("rom size %d, Prog size %d, CHR size %d\n", filesize, p[4], p[8]);

    rom->prg_rom_data = rom->rom_start + 16;
    rom->chr_rom_data = rom->prg_rom_data + 16384 * rom->prg_rom_size;
    printf("Prog size %x\n", rom->prg_rom_data);
}

void unload_rom(struct rom *rom)
{
    close(rom->rom_fd);
}
