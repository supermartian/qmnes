/*
 * ppu.c
 * Copyright (C) 2014 martian <supermartian@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#include "ppu.h"


uint32_t color_table[] = {0x525252, 0xB40000, 0xA00000, 0xB1003D, 0x740069, 0x00005B, 0x00005F, 0x001840, 0x002F10, 0x084A08, 0x006700, 0x124200, 0x6D2800, 0x000000, 0x000000, 0x000000, 0xC4D5E7, 0xFF4000, 0xDC0E22, 0xFF476B, 0xD7009F, 0x680AD7, 0x0019BC, 0x0054B1, 0x006A5B, 0x008C03, 0x00AB00, 0x2C8800, 0xA47200, 0x000000, 0x000000, 0x000000, 0xF8F8F8, 0xFFAB3C, 0xFF7981, 0xFF5BC5, 0xFF48F2, 0xDF49FF, 0x476DFF, 0x00B4F7, 0x00E0FF, 0x00E375, 0x03F42B, 0x78B82E, 0xE5E218, 0x787878, 0x000000, 0x000000, 0xFFFFFF, 0xFFF2BE, 0xF8B8B8, 0xF8B8D8, 0xFFB6FF, 0xFFC3FF, 0xC7D1FF, 0x9ADAFF, 0x88EDF8, 0x83FFDD, 0xB8F8B8, 0xF5F8AC, 0xFFFFB0, 0xF8D8F8, 0x000000, 0x000000};

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
            break;
        case 4:
            ret = p->oam[p->oama];
            break;
        case 7:
            ret = vram_read(p, p->rV);
            p->rV += p->vraminc;
            break;
        default:
            ret = 0;
            break;
    }

    return ret;
}

void ppu_write_reg(struct ppu *p, uint16_t addr, uint8_t val)
{
    p->status |= val & 0x0F;
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
            p->rV += p->vraminc;
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

void ppu_render_scanline(struct ppu *p)
{
    // Background
    // Sprite
    int i;
    int k = 0;
    int current[8];
    int current_oam;
    // Linear search for sprites in current scanline, pretty much like the secondary OAM does
    for (i = 0; i < 64 && k < 8; i++) {
        if ((p->oam[i<<2] <= (p->scanline - 1)) &&
                ((p->scanline - 1) < p->oam[i<<2] + (8 << p->spritet))) {
            printf("k i %d %d %d %d\n", k, i, p->oam[i<<2], p->oam[(i<<2) + 3]);
            current[k] = i;
            k++;
            if (i == 0) {
                // Sprite 0 hit
                p->status |= 0x40;
            }
        }
    }

    if (k == 8) {
        p->status |= 0x20;
    }

    for (i = 0 ; i < k ; i++) {
        current_oam = current[i];
        if (p->spritet) {
            // 8x16
            memset(p->frame + ((p->scanline - 1) * 256 + p->oam[(current_oam<<2)+3]), random(), 8);
        } else {
            memset(p->frame + ((p->scanline - 1) * 256 + p->oam[(current_oam<<2)+3]), random(), 8);
            // 8x8
        }
    }
}

void ppu_run(struct ppu *p, struct cpu_6502 *c, uint8_t cycle)
{
    int i;
    int max_cycle = 340;

    // Pre-render scanline has a variable cycle count
    if (p->scanline == 0 && p->odd_frame) {
        max_cycle = 339;
    }

    if (p->scanline_cycle == 0 &&
            p->scanline == 242) {
        p->status |= 0x80;
        c->nmi = p->vbi;
    }

    p->scanline_cycle += cycle;

    // We do the actual rendering on the last ppu tick of a scanline
    // Since most of the games don't do the "mid-scanline" rendering, it just OK
    if (p->scanline_cycle >= max_cycle) {
        p->scanline_cycle = 0;
        p->scanline++;

        if (p->scanline < 241 && p->scanline > 0) {
            ppu_render_scanline(p);
        } else if (p->scanline == 261) {
            // A frame has been completed, render the pixels to screen
            BIT_SET(p->status, 7, 0);
            p->r(p->frame);
            memset(p->frame, 0, 61440 * sizeof(uint32_t));
            p->odd_frame = !p->odd_frame;
            p->scanline = 0;
        }
    }
}
