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
 * CPU initialization.
 *
 * */
void cpu_init(struct cpu_6502 *p, float cpu_freq)
{
    memset(p, 0, sizeof(struct cpu_6502));
    p->cycle = 0;
}

uint16_t cpu_op_fetch_addr(struct cpu_6502 *p, uint8_t opcode)
{
    // JMP is a special babe!
    // 0X4C Absolute
    // 0X6C Absolute indirect 
    if (opcode == 0x4C) {
        return addr_abs(p);
    } else if (opcode == 0X6C) {
        return addr_absidir(p);
    }

    uint8_t addr_mode = (opcode & 0x1C) >> 2;
    uint8_t cc = (opcode & 0x03);
    uint16_t ret;
    switch (cc) {
        case 1:
            switch (addr_mode) {
                case 0:
                    ret = addr_iidir(p);
                    break;
                case 1:
                    ret = addr_zp(p);
                    break;
                case 2:
                    ret = addr_imm8(p);
                    break;
                case 3:
                    ret = addr_abs(p);
                    break;
                case 4:
                    ret = addr_idiri(p);
                    break;
                case 5:
                    ret = addr_zpix(p);
                    break;
                case 6:
                    ret = addr_aix(p);
                    break;
                case 7:
                    ret = addr_aiy(p);
                    break;
                default:
                    return 0;
            }
            break;
        case 0:
        case 2:
            switch (addr_mode) {
                case 0:
                    ret = addr_imm8(p);
                    break;
                case 1:
                    ret = addr_zp(p);
                    break;
                case 2:
                    // Accumulator
                    break;
                case 3:
                    ret = addr_abs(p);
                    break;
                case 5:
                    ret = addr_zpix(p);
                    break;
                case 7:
                    ret = addr_aix(p);
                    break;
                default:
                    return 0;
            }
            break;
        default:
            return 0;
    }

    return ret;
}

void cpu_execute_op(struct cpu_6502 *p, uint8_t opcode)
{
    uint8_t op = opcode & 0xE3;
    uint8_t cc = (opcode & 0x03);

    uint16_t addr = cpu_op_fetch_addr(p, opcode);
    switch (op) {
        // ORA
        case 0x01:
            break;
        // AND
        case 0x21:
            break;
        // EOR
        case 0x41:
            break;
        // ADC 
        case 0x61:
            break;
        // STA 
        case 0x81:
            break;
        // LDA 
        case 0xA1:
            break;
        // CMP 
        case 0xC1:
            break;
        // SBC 
        case 0xE1:
            break;
        // ASL
        case 0x02:
            break;
        // ROL
        case 0x22:
            break;
        // LSR
        case 0x42:
            break;
        // ROR 
        case 0x62:
            break;
        // STX 
        case 0x82:
            break;
        // LDX 
        case 0xA2:
            break;
        // DEC 
        case 0xC2:
            break;
        // INC 
        case 0xE2:
            break;
        // BIT 
        case 0x20:
            break;
        // JMP 
        case 0x40:
            break;
        // JMP abs 
        case 0x60:
            break;
        // STY 
        case 0x80:
            break;
        // LDY 
        case 0xA0:
            break;
        // CPY 
        case 0xC0:
            break;
        // CPX 
        case 0xE0:
            break;
    }
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
