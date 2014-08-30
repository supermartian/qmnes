/*
 * ppu.c
 * Copyright (C) 2014 martian <supermartian@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#include "ppu.h"


static const uint32_t color_table[] = {
0x7C7C7C
,0x0000FC
,0x0000BC
,0x4428BC
,0x940084
,0xA80020
,0xA81000
,0x881400
,0x503000
,0x007800
,0x006800
,0x005800
,0x004058
,0x000000
,0x000000
,0x000000
,0xBCBCBC
,0x0078F8
,0x0058F8
,0x6844FC
,0xD800CC
,0xE40058
,0xF83800
,0xE45C10
,0xAC7C00
,0x00B800
,0x00A800
,0x00A844
,0x008888
,0x000000
,0x000000
,0x000000
,0xF8F8F8
,0x3CBCFC
,0x6888FC
,0x9878F8
,0xF878F8
,0xF85898
,0xF87858
,0xFCA044
,0xF8B800
,0xB8F818
,0x58D854
,0x58F898
,0x00E8D8
,0x787878
,0x000000
,0x000000
,0xFCFCFC
,0xA4E4FC
,0xB8B8F8
,0xD8B8F8
,0xF8B8F8
,0xF8A4C0
,0xF0D0B0
,0xFCE0A8
,0xF8D878
,0xD8F878
,0xB8F8B8
,0xB8F8D8
,0x00FCFC
,0xF8D8F8
,0x000000
,0x000000
};

uint16_t vram_addr(struct ppu *p, uint16_t addr)
{
    uint16_t ret = addr;
    if (addr >= 0x2400 && addr < 0x2800) {
        if (p->rom_mirroring == 0) {
            ret = addr - 0x400;
        }
    } else if (addr >= 0x2800 && addr < 0x2C00) {
        if (p->rom_mirroring == 1) {
            ret = addr - 0x800;
        }
    } else if (addr >= 0x2C00 && addr < 0x3000) {
        if (p->rom_mirroring == 0) {
            ret = addr - 0x800;
        } else {
            ret = addr - 0x400;
        }
    } else if (addr >= 0x3000 && addr < 0x3F00) {
        ret = vram_addr(p, addr - 0x1000);
    } else if (addr >= 0x3F20 && addr < 0x4000) {
        ret = addr & 0x3F1F;
    } else {
    }
    return ret;
}

uint8_t vram_read(struct ppu *p, uint16_t addr)
{
    uint16_t a = vram_addr(p, addr);
    if (a < 0x2000) {
        return p->rom_chr[a];
    } else if (a >= 0x2000 && a < 0x3000) {
        return p->vram1[a - 0x2000];
    } else if (a >= 0x3F00 && a < 0x3F20) {
        return p->vram2[a - 0x3F00];
    } else {
        printf("Impossible read %x\n", a);
    }
}

void vram_write(struct ppu *p, uint16_t addr, uint8_t val)
{
    uint16_t a = vram_addr(p, addr);
    if (a < 0x2000) {
    } else if (a >= 0x2000 && a < 0x3000) {
        p->vram1[a - 0x2000] = val;
    } else if (a >= 0x3F00 && a < 0x3F20) {
        p->vram2[a - 0x3F00] = val;
    } else {
        printf("Impossible write %x %x\n", a, val);
    }
}

void ppu_write_reg(struct ppu *p, uint16_t addr, uint8_t val)
{
    uint16_t vv = val;
    switch (addr) {
        case 0:
            p->basent = (vv & 0x3 << 10) | 0x2000;
            p->vraminc = (vv & 0x4) ? 32 : 1;
            p->spritet = (vv & 0x8) ? 0x1000 : 0;
            p->bgt = (vv & 0x10) ? 0x1000 : 0;
            p->spritesz = (vv & 0x20) ? 1 : 0;
            p->vbi = (vv >> 7) & 0x1;
            p->scroll = (p->scroll & 0xF3FF) | ((vv & 0x3) << 10);
            break;
        case 1:
            p->mask = val;
            break;
        case 3:
            p->oama = val;
            break;
        case 4:
            p->oam[p->oama] = val;
            break;
        case 5:
            if (!p->w_toggle) {
                p->scroll = (p->scroll & 0xFFE0) | (vv >> 3);
                p->rX = vv & 0x7;
            } else {
                p->scroll &= 0x8C1F;
                p->scroll |= (vv & 0x7) << 12 | (vv & 0xF8) << 2;
            }
            p->w_toggle ^= 1;
            break;
        case 6:
            if (!p->w_toggle) {
                p->scroll &= 0x00FF;
                p->scroll |= (vv & 0x3F) << 8;
            } else {
                p->scroll &= 0xFF00;
                p->scroll |= vv;
                p->addr = p->scroll;
            }
            p->w_toggle ^= 1;
            break;
        case 7:
            vram_write(p, p->addr, val);
            p->addr += p->vraminc;
            p->addr &= 0x3FFF;
            break;
    }
    p->status = (p->status & 0xF0) | (0x0F & val);
}

uint8_t ppu_read_reg(struct ppu *p, uint16_t addr)
{
    uint8_t ret = 0;
    switch (addr) {
        case 2:
            ret = p->status;
            p->w_toggle = 0;
            break;
        case 4:
            ret = p->oam[p->oama];
            break;
        case 7:
            ret = vram_read(p, p->addr);
            p->addr += p->vraminc;
            p->addr &= 0x3FFF;
            break;
        default:
            break;
    }

    return ret;
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
    p->scrollx = 0;
    p->scrolly = 0;
    p->scanline_cycle = 0;
    p->scanline = 0;
    p->odd_frame = 0;
    p->ppu_ready = 0;

    p->w_toggle = 0;
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
    if ((p->scroll & 0x1F) == 0x1F) {
        p->scroll &= 0xFFE0;
        p->scroll ^= 0x0400;
    } else {
        p->scroll++;
    }
}

void ppu_inc_y(struct ppu *p)
{
    if ((p->scroll & 0x7000) != 0x7000) {
        p->scroll += 0x1000;
    } else {
        p->scroll &= ~0x7000;
        int y = (p->scroll & 0x03E0) >> 5;
        if (y == 29) {
            y = 0;
            p->scroll ^= 0x0800;
        } else if (y == 31) {
            y = 0;
        } else {
            y++;
        }
        p->scroll = (p->scroll & ~0x03E0) | (y << 5);
    }
}

void ppu_render_scanline_background(struct ppu *p)
{
    uint32_t *line = p->current_scanline_frame;
    uint16_t scroll_addr;
    uint16_t fine_x = p->scrollx & 0x7;
    uint16_t tile_addr;
    uint16_t tile1;
    uint16_t tile2;
    uint16_t attr_addr;
    uint16_t attr;
    uint8_t palette;
    uint32_t color[4];

    int i, j;

    line -= p->rX;
    for (i = 0; i < 32; i++) {
        tile_addr = vram_read(p, 0x2000 | (0xFFF & p->scroll));
        tile_addr <<= 4;
        tile_addr |= p->bgt;
        tile_addr += (p->scroll >> 12) & 0x7;
        tile1 = vram_read(p, tile_addr);
        tile2 = vram_read(p, (tile_addr + 8));

        attr_addr =  ((0xC00 & p->scroll) + 0x2300) | 0xC0 | ((p->scroll >> 2) & 0x7) | ((p->scroll >> 4) & 0x38);
        attr = vram_read(p, attr_addr);
        palette = attr >> ((((p->scroll >> 5) & 0x2) | ((p->scroll >> 1) & 0x1)) << 1);
        palette &= 0x3;
        color[0] = 0;
        color[1] = color_table[p->vram2[(palette << 2) + 1]];
        color[2] = color_table[p->vram2[(palette << 2) + 2]];
        color[3] = color_table[p->vram2[(palette << 2) + 3]];

        for (j = 7; j >= 0; j--) {
            line[7 - j] = color[0x3 & ((tile1 >> j) | (tile2 >> j << 1))];
        }
        line += 8;
        ppu_inc_x(p);
    }
    // Reset x scroll
    p->scroll ^= 0x400;
    ppu_inc_y(p);
}

void ppu_render_scanline_sprite(struct ppu *p)
{
    // Sprite
    uint32_t *line = p->current_scanline_frame;
    int i, j;
    int k = 0;
    int current[8];
    uint16_t current_oam;
    uint16_t tile1, tile2;
    uint16_t tileaddr;
    uint32_t x;
    uint32_t color[4];
    // Linear search for sprites in current scanline, pretty much like the secondary OAM does
    for (i = 0; i < 64 && k < 8; i++) {
        if ((p->oam[i<<2] <= (p->scanline - 1)) &&
                ((p->scanline - 1) < p->oam[i<<2] + (8 << p->spritesz))) {
            current[k] = i;
            k++;
        }
    }

    if (k == 8) {
        p->status |= 0x20;
    }

    uint8_t fx, fy;
    uint16_t palette;

    for (i = 0 ; i < k ; i++) {
        current_oam = current[i];
        x = p->oam[(current_oam<<2) + 3];
        fx = p->oam[(current_oam<<2) + 2] & 0x40;
        fy = p->oam[(current_oam<<2) + 2] & 0x80;

        palette = p->oam[(current_oam<<2) + 2] & 0x3;
        color[0] = 0;
        color[1] = color_table[vram_read(p, (palette<<2) + 0x3F11)];
        color[2] = color_table[vram_read(p, (palette<<2) + 0x3F12)];
        color[3] = color_table[vram_read(p, (palette<<2) + 0x3F13)];
        if (p->spritesz) {
            // 8x16
        } else {
            tileaddr = p->oam[(current_oam<<2) + 1];
            tileaddr = (tileaddr << 4);
            tileaddr = p->spritet ? tileaddr | 0x1000 : tileaddr;
            if (fy) {
                tileaddr += 8 - (p->scanline - 1 - p->oam[current_oam<<2]);
            } else {
                tileaddr += p->scanline - 1 - p->oam[current_oam<<2];
            }
            tile1 = vram_read(p, tileaddr);
            tile2 = vram_read(p, tileaddr + 8);
            if (fx) {
                for (j = 7; j >=0; j--){
                    line[x + j] = color[0x3 & ((tile1 >> j) | (tile2 >> j << 1))];
                    if (i == 0 && !line[x+j] && (p->mask & 0x08)) {
                        // Sprite 0 hit
                        p->status |= 0x40;
                    }
                }
            } else {
                for (j = 7; j >=0; j--){
                    line[x + 7 - j] = color[0x3 & ((tile1 >> j) | (tile2 >> j << 1))];
                    if (i == 0 && !line[x+7-j] && (p->mask & 0x08)) {
                        // Sprite 0 hit
                        p->status |= 0x40;
                    }
                }
            }
        }
    }
}

void nametabel_dump(struct ppu *p)
{
    int i, j, k;
    printf("==============\n");
    for (k = 0; k < 4; k++) {
    for (i = 0; i < 30; i++) {
        for (j = 0; j < 32; j++) {
            printf("%2x ", p->vram1[k * 32*32 + i*32+j]);
        }
        printf("\n");
    }
        printf("==================[%d]=================\n", k);
    }
}

void ppu_run(struct ppu *p, struct cpu_6502 *c, uint8_t cycle)
{
    int i;
    int max_cycle = 340;

    if (!p->ppu_ready) {
        return;
    }

    // Pre-render scanline has a variable cycle count
    if (p->scanline == 0 && p->odd_frame) {
        max_cycle = 339;
    }

    if (p->scanline_cycle == 0 && p->scanline == 0) {
        p->status = 0;
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

        if (p->scanline == 0) {
        } else if (p->scanline < 241 && p->scanline > 0) {
            p->current_scanline_frame = p->frame + (p->scanline - 1) * 256;
            if (p->mask & 0x08) {
                ppu_render_scanline_background(p);
                p->addr = p->scroll;
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
        }
        p->scanline_cycle = 0;
        p->scanline++;
    }
}
