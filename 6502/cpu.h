/*
 * cpu.h
 * Copyright (C) 2014 martian <supermartian@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef __CPU_H__
#define __CPU_H__

#include <stdint.h>

#include "ppu.h"
/*
 * NES 2A03 CPU memory map
 *
 * $0000–$00FF 256 bytes   Zero Page — Special Zero Page addressing modes give faster memory read/write access
 * $0100–$01FF 256 bytes   Stack memory
 * $0200–$07FF 1536 bytes  RAM
 * $0800–$0FFF 2048 bytes  Mirror of $0000–$07FF   $0800–$08FF Zero Page
 * $0900–$09FF Stack
 * $0A00–$0FFF RAM
 * $1000–$17FF 2048 bytes  Mirror of $0000–$07FF   $1000–$10FF Zero Page
 * $1100–$11FF Stack
 * $1200–$17FF RAM
 * $1800–$1FFF 2048 bytes  Mirror of $0000–$07FF   $1800–$18FF Zero Page
 * $1900–$19FF Stack
 * $1A00–$1FFF RAM
 * $2000–$2007 8 bytes Input/Output registers
 * $2008–$3FFF 8184 bytes  Mirror of $2000–$2007 (multiple times)
 * $4000–$401F 32 bytes    Input/Output registers
 * $4020–$5FFF 8160 bytes  Expansion ROM — Used with Nintendo's MMC5 to expand the capabilities of VRAM.
 * $6000–$7FFF 8192 bytes  SRAM — Save Ram used to save data between game plays.
 * $8000–$FFFF 32768 bytes PRG-ROM
 * $FFFA–$FFFB 2 bytes Address of Non Maskable Interrupt (NMI) handler routine
 * $FFFC–$FFFD 2 bytes Address of Power on reset handler routine
 * $FFFE–$FFFF 2 bytes Address of Break (BRK instruction) handler routine
 *
 * */

#define ZERO_START  0x0000
#define STACK_START 0x0100
#define PROG_START 0x0200
#define MIRROR1 0x0800
#define MIRROR2 0x1000
#define MIRROR3 0x1800
#define IO_START 0x4000
#define MMC5_START 0x4020
#define SRAM_START 0x6000
#define PRGROM_START 0x8000

#define CPU_FREQ_NTSC 1789772.5
#define CPU_FREQ_PAL  1773447.4

#define P_N 7
#define P_V 6
#define P_u 5
#define P_B 4
#define P_D 3
#define P_I 2
#define P_Z 1
#define P_C 0

typedef enum {
    KEYPAD_A = 0,
    KEYPAD_B,
    KEYPAD_SELECT,
    KEYPAD_START,
    KEYPAD_UP,
    KEYPAD_DOWN,
    KEYPAD_LEFT,
    KEYPAD_RIGHT
} keypad_state;

typedef void (handle_input)(struct cpu_6502 *p);

struct cpu_6502 {
    uint8_t ram[0x0800];
    uint16_t rPC;
    uint8_t rS;
    uint8_t rP;
    uint8_t rA;
    uint8_t rX;
    uint8_t rY;

    uint8_t nmi, prev_nmi;
    uint8_t irq, prev_irq;
    uint8_t rst, prev_rst;

    uint32_t cycle;
    uint32_t ins_cnt;

    uint8_t *rom_prg;

    struct ppu *ppu;
    handle_input *handle_input;
    uint8_t keypad[8];
    uint8_t strobe;
    uint8_t keynow;
};

void page_boundary_chk(struct cpu_6502 *p, uint16_t addr);
uint16_t mem_addr(uint16_t addr);
uint8_t mem_read(struct cpu_6502 *p, uint16_t addr);
uint8_t mem_write(struct cpu_6502 *p, uint16_t addr, uint8_t val);
void stack_push(struct cpu_6502 *p, uint8_t val);
uint8_t stack_pop(struct cpu_6502 *p);
uint8_t set_rp(struct cpu_6502 *p, uint8_t bit, uint8_t val);
uint8_t get_rp(struct cpu_6502 *p, uint8_t bit);
uint8_t read_rp(struct cpu_6502 *p);
void write_rp(struct cpu_6502 *p, uint8_t val);

void cpu_dump(struct cpu_6502 *p);

uint16_t addr_imm8(struct cpu_6502 *p);
uint16_t addr_rela(struct cpu_6502 *p);
uint16_t addr_abs(struct cpu_6502 *p);
uint16_t addr_zp(struct cpu_6502 *p);
uint16_t addr_aix(struct cpu_6502 *p);
uint16_t addr_aiy(struct cpu_6502 *p);
uint16_t addr_zpix(struct cpu_6502 *p);
uint16_t addr_zpiy(struct cpu_6502 *p);
uint16_t addr_idiri(struct cpu_6502 *p);
uint16_t addr_iidir(struct cpu_6502 *p);
uint16_t addr_absidir(struct cpu_6502 *p);

void joystick_write(struct cpu_6502 *p, uint8_t val);
uint8_t joystick_read(struct cpu_6502 *p);

void cpu_handle_intr(struct cpu_6502 *p);
void cpu_setup(struct cpu_6502 *p);
void cpu_reset(struct cpu_6502 *p);
uint16_t cpu_op_fetch_addr(struct cpu_6502 *p, uint8_t opcode);
uint8_t cpu_execute_op(struct cpu_6502 *p, uint8_t opcode);
void cpu_run(struct cpu_6502 *p);

#endif /* !__CPU_H__ */

