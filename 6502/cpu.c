/*
 * cpu.c
 * Copyright (C) 2014 martian <supermartian@gmail.com>
 *
 * Distributed under terms of the MIT license.
 * */

#include <time.h>
#include <string.h>
#include <stdint.h>

#include "cpu.h"
#include "ins.h"

// All the addressing functions return the address of the prospective memory.

/*
 * Immediate addressing.
 * */
uint16_t addr_imm8(struct cpu_6502 *p)
{
    return p->rPC++;
}

/*
 * Relative addressing.
 * */
uint16_t addr_rela(struct cpu_6502 *p)
{
    uint16_t ret = mem_read(p, p->rPC);
    ret += p->rPC;
    p->rPC++;
    // Relative addressing is only used by branch instructions.
    // Any page crossing issue will be dealt inside the instruction function.
    return ret;
}

/*
 * Absolute addressing.
 * */
uint16_t addr_abs(struct cpu_6502 *p)
{
    uint16_t ret = mem_read(p, p->rPC);
    p->rPC++;
    ret |= mem_read(p, p->rPC) << 8;
    p->rPC++;
    return ret;
}

/*
 * Zero page addressing.
 * */
uint16_t addr_zp(struct cpu_6502 *p)
{
    uint16_t ret = mem_read(p, p->rPC);
    p->rPC++;
    return ret;
}

/*
 * Absolute indexed by X addressing.
 * */
uint16_t addr_aix(struct cpu_6502 *p)
{
    uint16_t ret = mem_read(p, p->rPC);
    p->rPC++;
    ret |= mem_read(p, p->rPC) << 8;
    p->rPC++;
    ret += p->rX;
    page_boundary_chk(p, ret);
    return ret;
}

/*
 * Absolute indexed by Y addressing.
 * */
uint16_t addr_aiy(struct cpu_6502 *p)
{
    uint16_t ret = mem_read(p, p->rPC);
    p->rPC++;
    ret |= mem_read(p, p->rPC) << 8;
    p->rPC++;
    ret += p->rY;
    page_boundary_chk(p, ret);
    return ret;
}

/*
 * Zero page indexed by X addressing.
 * */
uint16_t addr_zpix(struct cpu_6502 *p)
{
    uint16_t ret = mem_read(p, p->rPC);
    p->rPC++;
    return ret + p->rX;
}

/*
 * Zero page indexed by Y addressing.
 * */
uint16_t addr_zpiy(struct cpu_6502 *p)
{
    uint16_t ret = mem_read(p, p->rPC);
    p->rPC++;
    return ret + p->rY;
}

/*
 * Indexed indirect addressing.
 * */
uint16_t addr_idiri(struct cpu_6502 *p)
{
    uint16_t ret = mem_read(p, p->rPC);
    p->rPC++;
    ret += p->rX;
    ret = mem_read(p, ret) | (mem_read(p, ret + 1) << 8);
    return ret;
}

/*
 * Indirect indexed addressing.
 * */
uint16_t addr_iidir(struct cpu_6502 *p)
{
    uint16_t ret = mem_read(p, p->rPC);
    p->rPC++;
    ret = mem_read(p, ret) | (mem_read(p, ret + 1) << 8);
    ret += p->rY;
    page_boundary_chk(p, ret);
    return ret;
}

/*
 * Absolute indirect addressing.
 * */
uint16_t addr_absidir(struct cpu_6502 *p)
{
    uint16_t ret = mem_read(p, p->rPC);
    p->rPC++;
    ret |= mem_read(p, p->rPC) << 8;
    p->rPC++;
    ret = mem_read(p, ret) | (mem_read(p, ret + 1) << 8);
    return ret;
}

/*
 * Some CPU configuration before power on.
 *
 * */
void cpu_setup(struct cpu_6502 *p)
{
    memset(p, 0, sizeof(struct cpu_6502));
    p->cycle = 0;
    ins_table_init();
    printf("instruction init\n");
}

/*
 * RESET the CPU.
 *
 * */
void cpu_reset(struct cpu_6502 *p)
{
    uint16_t start_addr;
    memset(p->ram, 0xFF, 0x0800 * sizeof(uint8_t));
    p->cycle = 0;

    start_addr = mem_read(p, 0xFFFC);
    start_addr |= mem_read(p, 0xFFFD) << 8;

    p->rPC = start_addr;
    p->rS = 0xFD;
    p->rA = 0;
    p->rX = 0;
    p->rY = 0;
    write_rp(p, 0x20);
}

void cpu_execute_op(struct cpu_6502 *p, uint8_t opcode)
{
    struct ins *i = &(ins_table[opcode]);
    uint16_t addr = 0;
    switch (i->addr) {
        case IMPL_ADDR:
            break;
        case ACCU_ADDR:
            break;
        case RELA_ADDR:
            addr = addr_rela(p);
            break;
        case IMM8_ADDR:
            addr = addr_imm8(p);
            break;
        case ABSO_ADDR:
            addr = addr_abs(p);
            break;
        case ZP_ADDR:
            addr = addr_zp(p);
            break;
        case AIX_ADDR:
            addr = addr_aix(p);
            break;
        case AIY_ADDR:
            addr = addr_aiy(p);
            break;
        case ZPIX_ADDR:
            addr = addr_zpix(p);
            break;
        case ZPIY_ADDR:
            addr = addr_zpiy(p);
            break;
        case IIDIR_ADDR:
            addr = addr_iidir(p);
            break;
        case IDIRI_ADDR:
            addr = addr_idiri(p);
            break;
        case ABSI_ADDR:
            addr = addr_absidir(p);
            break;
        default:
            addr = 0;
            break;
    }

    i->f(p, addr);
    p->cycle += i->cycle;
}

void cpu_dump(struct cpu_6502 *p)
{
    printf("========================================================\n");
    // dump memory
    /*
     *int i, j;
     *for (i = 0; i < 64; i++) {
     *    for (j = 0; j < 32; j++) {
     *        printf("%x ", p->ram[i*j]);
     *    }
     *    printf("\n");
     *}
     */

    printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");

    // dump registers
    printf("PC: %x - %x\t", p->rPC, mem_read(p, p->rPC));
    printf("SP: %x\t", p->rS);
    printf("A: %x\n", p->rA);
    printf("X: %x\t", p->rX);
    printf("Y: %x\n", p->rY);
    printf("P: %x\n", read_rp(p));

    // current cycle
    printf("In cycle: %d\n", p->cycle);
    printf("========================================================\n");
}

void cpu_run(struct cpu_6502 *p)
{
    uint8_t opcode;
    uint8_t op1;
    uint8_t op2;
    for(;;)
    {
        cpu_dump(p);
        opcode = mem_read(p, p->rPC);
        p->rPC++;
        cpu_execute_op(p, opcode);
    }
}

/*
 * Deal with page boundary crossing.
 * */
void page_boundary_chk(struct cpu_6502 *p, uint16_t addr)
{
    if ((p->rPC >> 8) != (addr >> 8)) {
        p->cycle++;
    }
}

/*
 * Deal with memory mirroring.
 * */
uint16_t mem_addr(uint16_t addr)
{
    if (addr < 0x2000) {
        return addr % 0x0800;
    } else if (addr >= 0x2000 && addr < 0x4000) {
        return ((addr - 0x2000) % 0x8) + 0x2000;
    } else {
        return addr;
    }
}

/*
 * General memory reading.
 * */
uint8_t mem_read(struct cpu_6502 *p, uint16_t addr)
{
    uint16_t addr_ = mem_addr(addr);
    if (addr_ < 0x0800) {
        printf("Reading ram %x\n", addr_);
        return p->ram[addr_];
    } else if (addr_ >= 0x8000) {
        printf("Reading cartiage %x, %x\n", addr_-0x8000, p->rom_prg[addr_ - 0x8000]);
        return p->rom_prg[addr_ - 0x8000];
    }
    return 0;
}

/*
 * General memory writing.
 * */
uint8_t mem_write(struct cpu_6502 *p, uint16_t addr, uint8_t val)
{
    uint16_t addr_ = mem_addr(addr);
    if (addr_ < 0x0800) {
        printf("writing ram %x\n", addr_);
        p->ram[addr_] = val;
    } else if (addr_ >= 0x8000) {
        printf("writing cartiage %x\n", addr_ - 0x8000);
        //p->rom_prg[addr_ - 0x8000] = val;
    }
    return 0;
}

/*
 * Push a value into stack.
 * */
void stack_push(struct cpu_6502 *p, uint8_t val)
{
    p->ram[p->rS + STACK_START] = val;
    p->rS--;
}

/*
 * Pop a value out of stack.
 * */
uint8_t stack_pop(struct cpu_6502 *p)
{
    uint8_t ret = p->ram[p->rS + STACK_START];
    p->rS++;
    return ret;
}

uint8_t read_rp(struct cpu_6502 *p)
{
    uint8_t *pt = (uint8_t *)(&(p->rP));
    return *pt;
}

void write_rp(struct cpu_6502 *p, uint8_t val)
{
    uint8_t *pt = (uint8_t *)(&(p->rP));
    *pt = val;
}
