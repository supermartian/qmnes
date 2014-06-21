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
    page_boundary_chk(p, ret);
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
 * CPU initialization. Just set the frequency.
 *
 * */
void cpu_init(struct cpu_6502 *p, float cpu_freq)
{
    memset(p, 0, sizeof(struct cpu_6502));
    clock_gettime(CLOCK_REALTIME, &(p->last_tick));
    p->cycle = 0;
}

void cpu_run(struct cpu_6502 *p)
{
    uint8_t opcode;
    uint8_t op1;
    uint8_t op2;
    for(;;)
    {
        opcode = mem_read(p->rPC);
        p->rPC++;
    }
}
