/*
 * ppu.c
 * Copyright (C) 2014 martian <supermartian@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#include "ppu.h"


uint32_t color_table[] = {0x525252, 0xB40000, 0xA00000, 0xB1003D, 0x740069, 0x00005B, 0x00005F, 0x001840, 0x002F10, 0x084A08, 0x006700, 0x124200, 0x6D2800, 0x000000, 0x000000, 0x000000, 0xC4D5E7, 0xFF4000, 0xDC0E22, 0xFF476B, 0xD7009F, 0x680AD7, 0x0019BC, 0x0054B1, 0x006A5B, 0x008C03, 0x00AB00, 0x2C8800, 0xA47200, 0x000000, 0x000000, 0x000000, 0xF8F8F8, 0xFFAB3C, 0xFF7981, 0xFF5BC5, 0xFF48F2, 0xDF49FF, 0x476DFF, 0x00B4F7, 0x00E0FF, 0x00E375, 0x03F42B, 0x78B82E, 0xE5E218, 0x787878, 0x000000, 0x000000, 0xFFFFFF, 0xFFF2BE, 0xF8B8B8, 0xF8B8D8, 0xFFB6FF, 0xFFC3FF, 0xC7D1FF, 0x9ADAFF, 0x88EDF8, 0x83FFDD, 0xB8F8B8, 0xF5F8AC, 0xFFFFB0, 0xF8D8F8, 0x000000, 0x000000};

uint16_t vram_addr(struct ppu *p, uint16_t addr)
{

    if (addr >= 0x2000 && addr < 0x3000) {
        switch (p->rom_mirroring) {
            case 0:
                // vertical arrangement/horizontal mirroring
                return addr & 0x2BFF;
            case 1:
                // horizontal arrangement/vertical mirroring
                return addr & 0x27FF;
            default:
                // HOLY SHIT might be four screen mirroring
                return addr;
        }
    } else if (addr >= 0x3000 && addr < 0x3F00) {
        return vram_addr(p, addr - 0x1000);
    } else if (addr >= 0x3F00 && addr < 0x4000) {
        return (addr - 0x3F00) % 0x20 + 0x3F00;
    } else {
        return addr;
    }
}

uint8_t vram_read(struct ppu *p, uint16_t addr)
{
    uint16_t a = vram_addr(p, addr);
    if (a < 0x2000) {
        return p->rom_chr[a];
    } else if (a >= 0x2000 && a < 0x3000) {
        return p->vram1[a - 0x2000];
    } else {
        return p->vram2[a - 0x3F00];
    }
}

void vram_write(struct ppu *p, uint16_t addr, uint8_t val)
{
    uint16_t a = vram_addr(p, addr);
    if (a < 0x2000) {
        p->rom_chr[a] = val;
    } else if (a >= 0x2000 && a < 0x3000){
        p->vram1[a - 0x2000] = val;
    } else {
        p->vram2[a - 0x3F00] = val;
    }
}

void ppu_setup(struct ppu *p)
{
    memset(p->vram1, 0xff, 0x1000 * sizeof(uint8_t));
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
    p->status |= val & 0x1F;
    uint16_t vv = val;
    switch (addr) {
        case 0:
            p->rT = val & 0xff;
            p->rT |= p->rT << 10;
            p->basent = 0x2000 | ((vv & 0x4) << 10);
            p->vraminc = 1 << ((vv & 0x4) << 5);
            p->bgt = (vv & 0x10) >> 1;
            p->spritet = !!(vv & 0x20);
            p->vbi = vv >> 7;
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
                p->rT |= (vv & 0xff) >> 3;
                p->rX = vv & 0x7;
            } else {
                p->rT |= (vv & 0x7) << 12;
                p->rT |= (vv & 0xF8) << 2;
            }
            p->w_toggle = !p->w_toggle;
            break;
        case 6:
            if (!p->w_toggle) {
                p->rT |= vv;
                p->rT = (p->rT << 8) & 0x3FFF;
            } else {
                p->rT |= vv;
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

void ppu_inc_x(struct ppu *p)
{
    int v = p->rV;
    if ((v & 0x001F) == 31) {
        v &= ~0x001F;
        v ^= 0x0400;
    } else {
        v += 1;
    }
    p->rV = v;
}

void ppu_inc_y(struct ppu *p)
{
    uint16_t y = (p->rV & 0x03E0) >> 5;
    if ((p->rV & 0x7000) != 0x7000) {
        p->rV += 0x1000;
    } else {
        p->rV &= ~0x7000;
        if (y == 29) {
            y = (p->rV & 0x03E0) >> 5;
            y = 0;
            p->rV  ^= 0x0800;
        } else if (y == 31) {
            y = 0;
        } else {
            y += 1;
            p->rV = (p->rV & ~0x03E0) | (y << 5);
        }
    }
    p->rV &= 0x0FFF;
}

uint16_t get_tile_addr(struct ppu *p)
{
    return 0x2000 | (p->rV & 0x0FFF);
}

uint16_t get_attr_addr(struct ppu *p)
{
    return 0x23C0 | (p->rV & 0x0C00) | ((p->rV >> 4) & 0x38) | ((p->rV >> 2) & 0x07);
}

void ppu_render_scanline_background(struct ppu *p)
{
    uint32_t *line = p->current_scanline_frame;
    uint8_t tile, attr;
    uint8_t tile_line1, tile_line2;
    int i, j, k;
    uint32_t pixelidx;
    uint32_t color[] = {0, 0xFFFF0000, 0xFF00FF00, 0xFF0000FF};

    line -= p->rX;
    for (i = 0; i < 32; i++) {
        tile = vram_read(p, get_tile_addr(p));
        attr = vram_read(p, get_attr_addr(p));
        //printf("tile addr %x\n", tile);
        tile_line1 = p->rom_chr[tile];
        tile_line2 = p->rom_chr[tile+8];
        for (j = 7; j > 0; j--) {
            pixelidx = tile_line1 >> j;
            pixelidx |= (tile_line2 >> j) << 1;
            pixelidx &= 0x3;
            line[j] = color[pixelidx];
        }
        ppu_inc_x(p);
        line += 8;
    }
    p->rV = p->rT;
//    printf("\n");
    ppu_inc_y(p);
}

void ppu_render_scanline_sprite(struct ppu *p)
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
            memset(p->frame + ((p->scanline - 1) * 256 + p->oam[(current_oam<<2)+3]), random(), 8*sizeof(uint32_t));
        } else {
            memset(p->frame + ((p->scanline - 1) * 256 + p->oam[(current_oam<<2)+3]), random(), 8*(sizeof(uint32_t)));
            // 8x8
        }
    }
}

void nametabel_dump(struct ppu *p)
{
    int i, j, k;
    printf("==============\n");
    for (i = 0; i < 30; i++) {
        for (j = 0; j < 32; j++) {
            printf("%2x ", p->vram1[i*32+j]);
        }
        printf("\n");
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
    // Since most of the games don't do the "mid-scanline" rendering, it's just OK
    if (p->scanline_cycle >= max_cycle) {

        if (p->scanline < 241 && p->scanline > 0) {
            p->current_scanline_frame = p->frame + (p->scanline - 1) * 256;
            if (p->mask & 0x08) {
                ppu_render_scanline_background(p);
            }
            if (p->mask & 0x10) {
                ppu_render_scanline_sprite(p);
            }
        } else if (p->scanline == 261) {
            // A frame has been completed, render the pixels to screen
            BIT_SET(p->status, 7, 0);
            p->r(p->frame);
            memset(p->frame, 0, 61440 * sizeof(uint32_t));
            p->odd_frame = !p->odd_frame;
            p->scanline = 0;
            //nametabel_dump(p);
        }
        p->scanline_cycle = 0;
        p->scanline++;
    }
}
