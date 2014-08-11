/*
 * ppu.c
 * Copyright (C) 2014 martian <supermartian@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#include "ppu.h"

uint16_t vram_addr(uint16_t addr)
{

    if (addr >= 0x3000 && addr < 0x3F00) {
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
    } else if (a >= 0x2000 && a < 0x3000) {
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
    p->scanline_cycle = 0;
    p->scanline = 0;
    p->fc = 0;
    p->odd_frame = 0;

    p->w_toggle = 0;
}

uint8_t ppu_read_reg(struct ppu *p, uint16_t addr)
{
    uint8_t ret;
    switch (addr) {
        case 2:
            p->w_toggle = 0;
            ret = p->status;
            BIT_SET(p->status, 7, 0);
        case 4:
            ret = p->oam[p->oama];
        case 7:
            ret = vram_read(p, p->rV);
        default:
            ret = 0;
    }

    return ret;
}

void ppu_write_reg(struct ppu *p, uint16_t addr, uint8_t val)
{
    switch (addr) {
        case 0:
            p->rT = val & 0xff;
            p->rT |= p->rT << 10;
            p->basent = 0x2000 | ((val & 0x4) << 10);
            p->vraminc = 1 << ((val & 0x4) << 5);
            p->bgt = (val & 0x10) >> 1;
            p->spritet = !!(val & 0x20);
            p->vbi = val >> 7;
            break;
        case 1:
            p->mask = val;
            break;
        case 3:
            p->oama = val;
            break;
        case 4:
            p->oam[p->oama] = val;
            p->oama++;
            break;
        case 5:
            if (!p->w_toggle) {
                p->rT |= (val & 0xff) >> 3;
                p->rX = val & 0x7;
            } else {
                p->rT |= (val & 0x7) << 12;
                p->rT |= (val & 0xF8) << 2;
            }
            p->w_toggle = !p->w_toggle;
            break;
        case 6:
            if (!p->w_toggle) {
                p->rT |= (0x3F & val) << 8;
                p->rT &= 0x3FFF;
            } else {
                p->rT |= val;
                p->rV = p->rT;
            }
            p->w_toggle = !p->w_toggle;
            break;
        case 7:
            vram_write(p, p->rV, val);
            break;
        default:
            break;
            // do shit
    }
}

void ppu_dma(struct ppu *p, struct cpu_6502 *c, uint8_t val)
{
    uint16_t addr = val<<8;
    int i = 0;
    for (i = 0; i < 256; i++) {
        p->oam[p->oama] = mem_read(c, addr);
        p->oama++;
        addr++;
    }
    c->cycle += 256;
}

void ppu_run(struct ppu *p, struct cpu_6502 *c, uint8_t cycle)
{
    int i;
    int max_cycle = 340;
    p->scanline_cycle += cycle;

    // Pre-render line
    if (p->odd_frame && p->scanline == 0) {
        BIT_SET(p->status, 6, 0);
        max_cycle = 339;
    }

    if (p->scanline_cycle > max_cycle) {
        p->scanline_cycle = 0;
        p->scanline++;
    }

    if (p->scanline == 242) {
        BIT_SET(p->status, 7, 1);
        c->nmi = 1;
    }

    if (p->scanline > 261) {
        p->fc++;
        if (p->fc == 60) {
            printf("frame \n");
            p->fc = 0;
        }
        p->r(p->frame);
        p->odd_frame = !p->odd_frame;
        p->scanline = 0;
    }
}
