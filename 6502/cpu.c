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
void cpu_setup(struct cpu_6502 *p, float cpu_freq)
{
    memset(p, 0, sizeof(struct cpu_6502));
    p->cycle = 0;
    ins_table_init();
}

/*
 * RESET the CPU.
 *
 * */
void cpu_reset(struct cpu_6502 *p)
{
    uint16_t start_addr;
    memset(p->ram, 0xFF, 0x0800);
    p->cycle = 0;

    start_addr = mem_read(p, 0xFFFC);
    start_addr |= mem_read(p, 0xFFFD) << 8;

    p->rPC = start_addr;
    p->rS = 0xFD;
    p->rA = 0;
    p->rX = 0;
    p->rY = 0;
    p->rP = 0x20;
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
    int i, j;
    for (i = 0; i < 32; i++) {
        for (j = 0; j < 64; j++) {
            printf("%x ", p->ram[i*j]);
        }
        printf("\n");
    }

    printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");

    // dump registers
    printf("PC: %x\t", p->rPC);
    printf("SP: %x\t", p->rS);
    printf("A: %x\n", p->rA);
    printf("X: %x\t", p->rX);
    printf("Y: %x\n", p->rY);

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
        opcode = mem_read(p, p->rPC);
        p->rPC++;
        cpu_execute_op(p, opcode);
    }
}
