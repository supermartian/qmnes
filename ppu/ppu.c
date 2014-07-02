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

void ppu_setup(struct ppu *p)
{
    memset(p->vram1, 0xff, 0x0f00 * sizeof(uint8_t));
    memset(p->vram2, 0, 0x0020 * sizeof(uint8_t));

    p->ctl = 0;
    p->mask = 0;
    p->status = 0;
    p->oama = 0;
    p->oamd = 0;
    p->scroll = 0;
    p->addr = 0;
    p->addr = 0;
}

uint8_t ppu_read_reg(struct ppu *p, uint16_t addr)
{
    uint8_t *r = p;
    return r[addr];
}

void ppu_write_reg(struct ppu *p, uint16_t addr, uint8_t val)
{
    uint8_t *r = p;
    return r[addr];
}
