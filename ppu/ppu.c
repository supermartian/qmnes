/*
 * ppu.c
 * Copyright (C) 2014 martian <supermartian@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#include "ppu.h"

uint16_t vram_addr(uint16_t addr)
{

    if (addr >= 0x2000 && addr < 0x3F00) {
        return addr - 0x1000;
    } else if (addr >= 0x3F00 && addr < 0x4000) {
        return (addr - 0x3F00) % 0x20 + 0x3F00;
    } else {
        return addr;
    }
}

uint8_t vram_read(struct ppu *p, uint16_t addr)
{
    uint16_t a = vram_addr(addr);
    if (a < 0x2000) {
        return p->rom_chr[a];
    } else if (a >= 0x2000 && a < 0x3000){
        return p->vram1[a];
    } else {
        return p->vram2[a - 0x3F00];
    }
}

void vram_write(struct ppu *p, uint16_t addr, uint8_t val)
{
    uint16_t a = vram_addr(addr);
    if (a < 0x2000) {
        p->rom_chr[a] = val;
    } else if (a >= 0x2000 && a < 0x3000){
        p->vram1[a] = val;
    } else {
        p->vram2[a - 0x3F00] = val;
    }
}

