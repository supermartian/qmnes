/*
 * test.c
 * Copyright (C) 2014 martian <supermartian@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#include <stdio.h>

#include "cpu.h"
#include "rom.h"
#include "ins.h"
#include "ppu.h"

int main(int argc, char **argv)
{
    if (argc != 2) {
        printf("oops\n");
        return 1;
    }

    char *filename = argv[1];

    struct rom test_rom;
    struct cpu_6502 cpu;
    struct ppu ppu;

    cpu_setup(&cpu);
    ppu_setup(&ppu);
    cpu.ppu = &ppu;

    load_rom(&test_rom, filename);
    printf("Prog size %x\n", test_rom.prg_rom_data);
    cpu.rom_prg = test_rom.prg_rom_data;
    ppu.rom_chr = test_rom.chr_rom_data;
    printf("Prog size %x\n", cpu.rom_prg);

    cpu_reset(&cpu);
    cpu_run(&cpu);

    return 0;
}
