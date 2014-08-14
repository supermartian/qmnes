/*
 * ppu.h
 * Copyright (C) 2014 martian <supermartian@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef __PPU_H__
#define __PPU_H__

#include "cpu.h"

#include <stdint.h>
/*
 * PPU memory map
 *
 * PPU VRAM memory map
 * Address range Size Description
 * $0000-$0FFF $1000Pattern Table 0 [lower CHR bank]
 * $1000-$1FFF $1000 Pattern Table 1 [upper CHR bank]
 * $2000-$23FF $0400Name Table #0
 * $2400-$27FF $0400Name Name Table #1
 * $2800-$2BFF $0400Name Name Table #2
 * $2C00-$2FFF $0400Name Name Table #3
 * $3000-$3EFF $0F00 Mirrors of $2000-$2EFF
 * $3F00-$3F1F $0020Palette RAM indexes [not RGB values]
 * $3F20-$3FFF $0080 Mirrors of $3F00-$3F1F
 *
 * Memory details
 * Address Size Note Description
 * $0000 $1000 CHR Pattern Table #0
 * $1000 $1000 CHR Pattern Table #1
 * $2000$03C0 Name Table #0
 * $23C0$0040 Attribute Table #0
 * $2400$03C0 Name Name Table #1
 * $27C0$0040 Name Attribute Table #1
 * $2800$03C0 Name Name Table #2
 * $2BC0$0040 Name Attribute Table #2
 * $2C00$03C0 Name Name Table #3
 * $2FC0$0040 Name Attribute Table #3
 * $3000$0F00 R
 * $3F00$0010 Background palette
 * $3F10$0010 Sprite palette
 * $3F20$00E0 P
 *
 * */

typedef void (renderer)(uint32_t *pixels);

struct ppu {
    uint8_t ctl;                // write
    uint8_t mask;               // write
    uint8_t status;             // read 
    uint8_t oama;               // write
    uint8_t oamd;               // write/read
    uint16_t scroll;             // write * 2
    uint16_t addr;               // write * 2
    uint8_t data;               // write/read

    uint16_t basent;
    uint8_t vraminc;
    uint8_t spritet;
    uint8_t bgt;
    uint8_t spritesz;
    uint8_t vbi;

    uint8_t vram1[0x1000];
    uint8_t vram2[0x0020];
    uint8_t oam[0x0100];

    uint8_t *rom_chr;
    uint8_t rom_mirroring;

    uint32_t frame[61440];
    uint32_t scanline;
    uint8_t odd_frame;

    uint32_t scanline_cycle;
    uint32_t *current_scanline_frame;

    uint16_t rV;
    uint16_t rT;
    uint16_t rX;
    uint8_t w_toggle;         // First write toggle for 2005 and 2006
    uint8_t sprite0;

    renderer *r;
};

uint16_t vram_addr(struct ppu *p, uint16_t addr);
uint8_t vram_read(struct ppu *p, uint16_t addr);
void vram_write(struct ppu *p, uint16_t addr, uint8_t val);
uint8_t ppu_read_reg(struct ppu *p, uint16_t addr);
void ppu_write_reg(struct ppu *p, uint16_t addr, uint8_t val);
void ppu_dma(struct ppu *p, struct cpu_6502 *c, uint8_t val);

uint16_t get_tile_addr(struct ppu *p);
uint16_t get_attr_addr(struct ppu *p);
void ppu_inc_x(struct ppu *p);
void ppu_inc_y(struct ppu *p);

void ppu_render_scanline_background(struct ppu *p);
void ppu_render_scanline_sprite(struct ppu *p);

void ppu_setup(struct ppu *p);
void ppu_run(struct ppu *p, struct cpu_6502 *c, uint8_t cycle);

void nametabel_dump(struct ppu *p);

#endif /* !__PPU_H__ */

