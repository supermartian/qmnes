/*
 * cpu.h
 * Copyright (C) 2014 martian <supermartian@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef __CPU_H__
#define __CPU_H__

#include <stdint.h>
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
 */

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

/*
 * Addressing Model marcos
 */
#define RELT_ADDR(cpu, op) (cpu->rPC + op)
#define ABS_ADDR(cpu, op1, op2) ((op2 << 8) | op1)
#define ZP_ADDR(cpu, op) op
#define AIX_ADDR(cpu, op1, op2) (((op2 << 8) | op1) + cpu->rX)
#define AIY_ADDR(cpu, op1, op2) (((op2 << 8) | op1) + cpu->rY)
#define ZPIX_ADDR(cpu, op) (op + cpu->rX)
#define ZPIY_ADDR(cpu, op) (op + cpu->rY)
#define IDIRI_ADDR(cpu, op) ((cpu->mem[op + cpu->rX]) | ((cpu->mem[op + cpu->rX + 1]) << 8))
#define IIDIR_ADDR(cpu, op) ((cpu->mem[op] | (cpu->mem[op + 1]) << 8) + cpu->rY)
#define ABSIDIR_ADDR(cpu, op1, op2) cpu->mem[((op2 << 8) | op1)] \
    | (cpu->mem[((op2 << 8) | op1) + 1] << 8)

/*
 * Deal with the memory mirroring.
 *
 */
inline uint16_t mem_addr(uint16_t addr)
{
    if (addr < 0x2000) {
        return addr % 0x0800;
    } else if (addr >= 0x2000 && addr < 0x4000) {
        return ((addr - 0x2000) % 0x8) + 0x2000;
    } else {
        return addr;
    }
}


struct cpu_6502 {
    uint8_t mem[0xffff];
    uint16_t rPC;
    uint8_t rS;
    struct rP {
        uint8_t N:1,
                V:1,
                unused:1,
                b:1,
                d:1,
                i:1,
                Z:1,
                C:1;
    };
    uint8_t rA;
    uint8_t rX;
    uint8_t rY;

    uint32_t cycle;
    struct timespec last_tick;
};

void cpu_init(struct cpu_6502 *p, float cpu_freq);
void cpu_cycle_tick(struct cpu_6502 *p, int cycles);

#endif /* !__CPU_H__ */

