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
 * CPU initialization.
 *
 * */
void cpu_init(struct cpu_6502 *p, float cpu_freq)
{
    memset(p, 0, sizeof(struct cpu_6502));
    p->cycle = 0;
    ins_table_init();
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
