/*
 * ins.c
 * Copyright (C) 2014 martian <supermartian@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#include <stdint.h>
#include "ins.h"

void ins_table_init()
{
    int i = 0;
    for (i = 0; i <= 0xff; i++) {
        INS_ADD(i, IMPL_ADDR, unknown, 0);
    }

    // ADC
    INS_ADD(0x69, IMM8_ADDR, adc, 2);
    INS_ADD(0x65, ZP_ADDR, adc, 3);
    INS_ADD(0x75, ZPIX_ADDR, adc, 4);
    INS_ADD(0x6D, ABSO_ADDR, adc, 4);
    INS_ADD(0x7D, AIX_ADDR, adc, 4);
    INS_ADD(0x79, AIY_ADDR, adc, 4);
    INS_ADD(0x61, IDIRI_ADDR, adc, 6);
    INS_ADD(0x71, IIDIR_ADDR, adc, 5);

    // AND 
    INS_ADD(0x29, IMM8_ADDR, and, 2);
    INS_ADD(0x25, ZP_ADDR, and, 2);
    INS_ADD(0x35, ZPIX_ADDR, and, 3);
    INS_ADD(0x2D, ABSO_ADDR, and, 4);
    INS_ADD(0x3D, AIX_ADDR, and, 4);
    INS_ADD(0x39, AIY_ADDR, and, 4);
    INS_ADD(0x21, IDIRI_ADDR, and, 6);
    INS_ADD(0x31, IIDIR_ADDR, and, 5);

    // ASL
    INS_ADD(0x0A, ACCU_ADDR, asla, 2);
    INS_ADD(0x06, ZP_ADDR, asl, 5);
    INS_ADD(0x16, ZPIX_ADDR, asl, 6);
    INS_ADD(0x0E, ABSO_ADDR, asl, 6);
    INS_ADD(0x1E, AIX_ADDR, asl, 7);

    // BCC
    INS_ADD(0x90, RELA_ADDR, bcc, 2);
    INS_ADD(0xB0, RELA_ADDR, bcs, 2);
    INS_ADD(0xF0, RELA_ADDR, beq, 2);

    INS_ADD(0x24, ZP_ADDR, bit, 3);
    INS_ADD(0x2C, ABSO_ADDR, bit, 4);

    INS_ADD(0x30, RELA_ADDR, bmi, 2);
    INS_ADD(0xD0, RELA_ADDR, bne, 2);
    INS_ADD(0x10, RELA_ADDR, bpl, 2);

    INS_ADD(0x00, IMPL_ADDR, brk, 7);

    INS_ADD(0x50, RELA_ADDR, bvc, 2);
    INS_ADD(0x70, RELA_ADDR, bvs, 2);

    INS_ADD(0x18, IMPL_ADDR, clc, 2);
    INS_ADD(0xD8, IMPL_ADDR, cld, 2);
    INS_ADD(0x58, IMPL_ADDR, cli, 2);
    INS_ADD(0xB8, IMPL_ADDR, clv, 2);

     // CMP 
    INS_ADD(0xC9, IMM8_ADDR, cmp, 2);
    INS_ADD(0xC5, ZP_ADDR, cmp, 3);
    INS_ADD(0xD5, ZPIX_ADDR, cmp, 4);
    INS_ADD(0xCD, ABSO_ADDR, cmp, 4);
    INS_ADD(0xDD, AIX_ADDR, cmp, 4);
    INS_ADD(0xD9, AIY_ADDR, cmp, 4);
    INS_ADD(0xC1, IDIRI_ADDR, cmp, 6);
    INS_ADD(0xD1, IIDIR_ADDR, cmp, 5);

    INS_ADD(0xE0, IMM8_ADDR, cpx, 2);
    INS_ADD(0xE4, ZP_ADDR, cpx, 3);
    INS_ADD(0xEC, ABSO_ADDR, cpx, 4);

    INS_ADD(0xC0, IMM8_ADDR, cpy, 2);
    INS_ADD(0xC4, ZP_ADDR, cpy, 3);
    INS_ADD(0xCC, ABSO_ADDR, cpy, 4);

    INS_ADD(0xC6, ZP_ADDR, dec, 5);
    INS_ADD(0xD6, ZPIX_ADDR, dec, 6);
    INS_ADD(0xCE, ABSO_ADDR, dec, 6);
    INS_ADD(0xDE, AIX_ADDR, dec, 7);

    INS_ADD(0xCA, IMPL_ADDR, dex, 2);
    INS_ADD(0x88, IMPL_ADDR, dey, 2);

     // EOR 
    INS_ADD(0x49, IMM8_ADDR, eor, 2);
    INS_ADD(0x45, ZP_ADDR, eor, 3);
    INS_ADD(0x55, ZPIX_ADDR, eor, 4);
    INS_ADD(0x4D, ABSO_ADDR, eor, 4);
    INS_ADD(0x5D, AIX_ADDR, eor, 4);
    INS_ADD(0x59, AIY_ADDR, eor, 4);
    INS_ADD(0x41, IDIRI_ADDR, eor, 6);
    INS_ADD(0x51, IIDIR_ADDR, eor, 5);

    // INC
    INS_ADD(0xE6, ZP_ADDR, inc, 5);
    INS_ADD(0xF6, ZPIX_ADDR, inc, 6);
    INS_ADD(0xEE, ABSO_ADDR, inc, 6);
    INS_ADD(0xFE, AIX_ADDR, inc, 7);

    INS_ADD(0xE8, IMPL_ADDR, inx, 2);
    INS_ADD(0xC8, IMPL_ADDR, iny, 2);

    INS_ADD(0x4C, ABSO_ADDR, jmp, 3);
    INS_ADD(0x6C, ABSI_ADDR, jmp, 5);

    INS_ADD(0x20, ABSO_ADDR, jsr, 6);

     // LDA 
    INS_ADD(0xA9, IMM8_ADDR, lda, 2);
    INS_ADD(0xA5, ZP_ADDR, lda, 3);
    INS_ADD(0xB5, ZPIX_ADDR, lda, 4);
    INS_ADD(0xAD, ABSO_ADDR, lda, 4);
    INS_ADD(0xBD, AIX_ADDR, lda, 4);
    INS_ADD(0xB9, AIY_ADDR, lda, 4);
    INS_ADD(0xA1, IDIRI_ADDR, lda, 6);
    INS_ADD(0xB1, IIDIR_ADDR, lda, 5);

    INS_ADD(0xA2, IMM8_ADDR, ldx, 2);
    INS_ADD(0xA6, ZP_ADDR, ldx, 3);
    INS_ADD(0xB6, ZPIY_ADDR, ldx, 4);
    INS_ADD(0xAE, ABSO_ADDR, ldx, 4);
    INS_ADD(0xBE, AIY_ADDR, ldx, 4);

    INS_ADD(0xA0, IMM8_ADDR, ldy, 2);
    INS_ADD(0xA4, ZP_ADDR, ldy, 3);
    INS_ADD(0xB4, ZPIX_ADDR, ldy, 4);
    INS_ADD(0xAC, ABSO_ADDR, ldy, 4);
    INS_ADD(0xBC, AIX_ADDR, ldy, 4);

    // LSR 
    INS_ADD(0x4A, ACCU_ADDR, lsra, 2);
    INS_ADD(0x46, ZP_ADDR, lsr, 5);
    INS_ADD(0x56, ZPIX_ADDR, lsr, 6);
    INS_ADD(0x4E, ABSO_ADDR, lsr, 6);
    INS_ADD(0x5E, AIX_ADDR, lsr, 7);

    INS_ADD(0xEA, IMPL_ADDR, nop, 2);

    // ORA 
    INS_ADD(0x09, IMM8_ADDR, ora, 2);
    INS_ADD(0x05, ZP_ADDR, ora, 2);
    INS_ADD(0x15, ZPIX_ADDR, ora, 3);
    INS_ADD(0x0D, ABSO_ADDR, ora, 4);
    INS_ADD(0x1D, AIX_ADDR, ora, 4);
    INS_ADD(0x19, AIY_ADDR, ora, 4);
    INS_ADD(0x01, IDIRI_ADDR, ora, 6);
    INS_ADD(0x11, IIDIR_ADDR, ora, 5);

    INS_ADD(0x48, IMPL_ADDR, pha, 3);
    INS_ADD(0x08, IMPL_ADDR, php, 3);
    INS_ADD(0x68, IMPL_ADDR, pla, 4);
    INS_ADD(0x28, IMPL_ADDR, plp, 4);

    // ROL 
    INS_ADD(0x2A, ACCU_ADDR, rola, 2);
    INS_ADD(0x26, ZP_ADDR, rol, 5);
    INS_ADD(0x36, ZPIX_ADDR, rol, 6);
    INS_ADD(0x2E, ABSO_ADDR, rol, 6);
    INS_ADD(0x3E, AIX_ADDR, rol, 7);

    // ROR 
    INS_ADD(0x6A, ACCU_ADDR, rora, 2);
    INS_ADD(0x66, ZP_ADDR, ror, 5);
    INS_ADD(0x76, ZPIX_ADDR, ror, 6);
    INS_ADD(0x6E, ABSO_ADDR, ror, 6);
    INS_ADD(0x7E, AIX_ADDR, ror, 7);

    INS_ADD(0x40, IMPL_ADDR, rti, 6);
    INS_ADD(0x60, IMPL_ADDR, rts, 6);

    // SBC 
    INS_ADD(0xE9, IMM8_ADDR, sbc, 2);
    INS_ADD(0xE5, ZP_ADDR, sbc, 3);
    INS_ADD(0xF5, ZPIX_ADDR, sbc, 4);
    INS_ADD(0xED, ABSO_ADDR, sbc, 4);
    INS_ADD(0xFD, AIX_ADDR, sbc, 4);
    INS_ADD(0xF9, AIY_ADDR, sbc, 4);
    INS_ADD(0xE1, IDIRI_ADDR, sbc, 6);
    INS_ADD(0xF1, IIDIR_ADDR, sbc, 5);

    INS_ADD(0x38, IMPL_ADDR, sec, 2);
    INS_ADD(0xF8, IMPL_ADDR, sed, 2);
    INS_ADD(0x78, IMPL_ADDR, sei, 2);

    // STA
    INS_ADD(0x85, ZP_ADDR, sta, 3);
    INS_ADD(0x95, ZPIX_ADDR, sta, 4);
    INS_ADD(0x8D, ABSO_ADDR, sta, 4);
    INS_ADD(0x9D, AIX_ADDR, sta, 5);
    INS_ADD(0x99, AIY_ADDR, sta, 5);
    INS_ADD(0x81, IDIRI_ADDR, sta, 6);
    INS_ADD(0x91, IIDIR_ADDR, sta, 6);

    // STX
    INS_ADD(0x86, ZP_ADDR, stx, 3);
    INS_ADD(0x96, ZPIY_ADDR, stx, 4);
    INS_ADD(0x8E, ABSO_ADDR, stx, 4);

    // STY
    INS_ADD(0x84, ZP_ADDR, sty, 3);
    INS_ADD(0x94, ZPIX_ADDR, sty, 4);
    INS_ADD(0x8C, ABSO_ADDR, sty, 4);

    INS_ADD(0xAA, IMPL_ADDR, tax, 2);
    INS_ADD(0xA8, IMPL_ADDR, tay, 2);
    INS_ADD(0xBA, IMPL_ADDR, tsx, 2);
    INS_ADD(0x8A, IMPL_ADDR, txa, 2);
    INS_ADD(0x9A, IMPL_ADDR, txs, 2);
    INS_ADD(0x98, IMPL_ADDR, tya, 2);

    // UNDOCUMENTED!
    // http://nesdev.com/undocumented_opcodes.txt
    // AAX
    INS_ADD(0x87, ZP_ADDR, aax, 3);
    INS_ADD(0x97, ZPIY_ADDR, aax, 4);
    INS_ADD(0x83, IDIRI_ADDR, aax, 6);
    INS_ADD(0x8f, ABSO_ADDR, aax, 4);

/*
 *
 *    // AAC
 *    INS_ADD(0x0B, IMM8_ADDR, aac, 2);
 *    INS_ADD(0x2B, IMM8_ADDR, aac, 2);
 *
*
 *    // ARR
 *    INS_ADD(0x6B, IMM8_ADDR, arr, 2);
 *
 *    // ASR
 *    INS_ADD(0x4B, IMM8_ADDR, asr, 2);
 *
 *    // ATX
 *    INS_ADD(0xAB, IMM8_ADDR, atx, 2);
 *
 *    // AXA
 *    INS_ADD(0x9F, AIY_ADDR, axa, 5);
 *    INS_ADD(0x93, IIDIR_ADDR, axa, 6);
 *
 *    // AXS
 *    INS_ADD(0xCB, IMM8_ADDR, axs, 2);
 *
 *    // DCP
 *    INS_ADD(0xC7, ZP_ADDR, dcp, 5);
 *    INS_ADD(0xD7, ZPIX_ADDR, dcp, 6);
 *    INS_ADD(0xCF, ABSO_ADDR, dcp, 6);
 *    INS_ADD(0xDF, AIX_ADDR, dcp, 7);
 *    INS_ADD(0xDB, AIY_ADDR, dcp, 7);
 *    INS_ADD(0xC3, IIDIR_ADDR, dcp, 8);
 *    INS_ADD(0xD3, IDIRI_ADDR, dcp, 8);
 */

    // DOP
    INS_ADD(0x04, ZP_ADDR, nop, 3);
    INS_ADD(0x14, ZPIX_ADDR, nop, 4);
    INS_ADD(0x34, ZPIX_ADDR, nop, 4);
    INS_ADD(0x44, ZP_ADDR, nop, 3);
    INS_ADD(0x54, ZPIX_ADDR, nop, 4);
    INS_ADD(0x64, ZP_ADDR, nop, 3);
    INS_ADD(0x74, ZPIX_ADDR, nop, 4);
    INS_ADD(0x80, IMM8_ADDR, nop, 2);
    INS_ADD(0x82, IMM8_ADDR, nop, 2);
    INS_ADD(0x89, IMM8_ADDR, nop, 2);
    INS_ADD(0xC2, IMM8_ADDR, nop, 2);
    INS_ADD(0xD4, ZPIX_ADDR, nop, 4);
    INS_ADD(0xE2, IMM8_ADDR, nop, 2);
    INS_ADD(0xF4, ZPIX_ADDR, nop, 4);

    // NOP
    INS_ADD(0x1A, IMPL_ADDR, nop, 2);
    INS_ADD(0x3A, IMPL_ADDR, nop, 2);
    INS_ADD(0x5A, IMPL_ADDR, nop, 2);
    INS_ADD(0x7A, IMPL_ADDR, nop, 2);
    INS_ADD(0xDA, IMPL_ADDR, nop, 2);
    INS_ADD(0xFA, IMPL_ADDR, nop, 2);

    // TOP
    INS_ADD(0x0C, ABSO_ADDR, nop, 4);
    INS_ADD(0x1C, AIX_ADDR, nop, 4);
    INS_ADD(0x3C, AIX_ADDR, nop, 4);
    INS_ADD(0x5C, AIX_ADDR, nop, 4);
    INS_ADD(0x7C, AIX_ADDR, nop, 4);
    INS_ADD(0xDC, AIX_ADDR, nop, 4);
    INS_ADD(0xFC, AIX_ADDR, nop, 4);

    // LAX
    INS_ADD(0xA7, ZP_ADDR, lax, 3);
    INS_ADD(0xB7, ZPIY_ADDR, lax, 4);
    INS_ADD(0xAF, ABSO_ADDR, lax, 4);
    INS_ADD(0xBF, AIY_ADDR, lax, 4);
    INS_ADD(0xA3, IDIRI_ADDR, lax, 6);
    INS_ADD(0xB3, IIDIR_ADDR, lax, 5);
}

void ins_adc(struct cpu_6502 *p, uint16_t addr)
{
    uint16_t t;
    uint8_t val = mem_read(p, addr);

    t = p->rA + val + get_rp(p, P_C);

    /* No BCD supported in NES */
    set_rp(p, P_C, (t & 0xff00) != 0);
    set_rp(p, P_V, (((p->rA ^ t) & (val ^ t)) & 0x80) ? 1 : 0);

    p->rA = t & 0xff;

    set_rp(p, P_Z, p->rA == 0);
    set_rp(p, P_N, p->rA >> 7);
}

void ins_and(struct cpu_6502 *p, uint16_t addr)
{
    uint8_t val = mem_read(p, addr);
    p->rA &= val;

    set_rp(p, P_N, p->rA >> 7);
    set_rp(p, P_Z, p->rA == 0);
}

void ins_asla(struct cpu_6502 *p, uint16_t addr)
{
    uint8_t tmp = p->rA;

    set_rp(p, P_C, tmp >> 7);
    tmp = (tmp << 1) & 0xfe;
    set_rp(p, P_N, tmp >> 7);
    set_rp(p, P_Z, tmp == 0);

    p->rA = tmp;
}

void ins_asl(struct cpu_6502 *p, uint16_t addr)
{
    uint8_t tmp = mem_read(p, addr);

    set_rp(p, P_C, tmp >> 7);
    tmp = (tmp << 1) & 0xfe;
    set_rp(p, P_N, tmp >> 7);
    set_rp(p, P_Z, tmp == 0);

    mem_write(p, addr, tmp);
}

void ins_bcc(struct cpu_6502 *p, uint16_t addr)
{
    uint16_t oldPC = p->rPC;
    if (!get_rp(p, P_C)) {
        p->rPC = addr;
        p->cycle++;
        page_boundary_chk(p, oldPC);
    }
}

void ins_bcs(struct cpu_6502 *p, uint16_t addr)
{
    uint16_t oldPC = p->rPC;
    if (get_rp(p, P_C)) {
        p->rPC = addr;
        p->cycle++;
        page_boundary_chk(p, oldPC);
    }
}

void ins_beq(struct cpu_6502 *p, uint16_t addr)
{
    uint16_t oldPC = p->rPC;
    if (get_rp(p, P_Z)) {
        p->rPC = addr;
        p->cycle++;
        page_boundary_chk(p, oldPC);
    }
}

void ins_bit(struct cpu_6502 *p, uint16_t addr)
{
    uint8_t t = mem_read(p, addr);
    set_rp(p, P_N, (t >> 7) & 0x01);
    set_rp(p, P_V, (t >> 6) & 0x01);
    set_rp(p, P_Z, (t & p->rA) == 0);
}

void ins_bmi(struct cpu_6502 *p, uint16_t addr)
{
    uint16_t oldPC = p->rPC;
    if (get_rp(p, P_N)) {
        p->rPC = addr;
        p->cycle++;
        page_boundary_chk(p, oldPC);
    }
}

void ins_bne(struct cpu_6502 *p, uint16_t addr)
{
    uint16_t oldPC = p->rPC;
    if (!get_rp(p, P_Z)) {
        p->rPC = addr;
        p->cycle++;
        page_boundary_chk(p, oldPC);
    }
}

void ins_bpl(struct cpu_6502 *p, uint16_t addr)
{
    uint16_t oldPC = p->rPC;
    if (!get_rp(p, P_N)) {
        p->rPC = addr;
        p->cycle++;
        page_boundary_chk(p, oldPC);
    }
}

void ins_brk(struct cpu_6502 *p, uint16_t addr)
{
    stack_push(p, p->rPC >> 8);
    stack_push(p, p->rPC & 0xFF);
    stack_push(p, (read_rp(p) | 0x10));

    set_rp(p, P_I, 1);
    if (p->nmi) {
        p->rPC = mem_read(p, 0xFFFA) | ((uint16_t)mem_read(p, 0xFFFB) << 8);
    } else {
        p->rPC = mem_read(p, 0xFFFE) | ((uint16_t)mem_read(p, 0xFFFF) << 8);
    }
}

void ins_bvc(struct cpu_6502 *p, uint16_t addr)
{
    uint16_t oldPC = p->rPC;
    if (!get_rp(p, P_V)) {
        p->rPC = addr;
        p->cycle++;
        page_boundary_chk(p, oldPC);
    }
}

void ins_bvs(struct cpu_6502 *p, uint16_t addr)
{
    uint16_t oldPC = p->rPC;
    if (get_rp(p, P_V)) {
        p->rPC = addr;
        p->cycle++;
        page_boundary_chk(p, oldPC);
    }
}

void ins_clc(struct cpu_6502 *p, uint16_t addr)
{
    set_rp(p, P_C, 0);
}

void ins_cld(struct cpu_6502 *p, uint16_t addr)
{
    set_rp(p, P_D, 0);
}

void ins_cli(struct cpu_6502 *p, uint16_t addr)
{
    set_rp(p, P_I, 0);
}

void ins_clv(struct cpu_6502 *p, uint16_t addr)
{
    set_rp(p, P_V, 0);
}

void ins_cmp(struct cpu_6502 *p, uint16_t addr)
{
    uint8_t val = mem_read(p, addr);
    uint8_t t = p->rA - val;
    set_rp(p, P_C, p->rA >= val);
    set_rp(p, P_N, t >> 7);
    set_rp(p, P_Z, t == 0);
}

void ins_cpx(struct cpu_6502 *p, uint16_t addr)
{
    uint8_t val = mem_read(p, addr);
    uint8_t t = p->rX - val;
    set_rp(p, P_N, t >> 7);
    set_rp(p, P_C, p->rX >= val);
    set_rp(p, P_Z, t == 0);
}

void ins_cpy(struct cpu_6502 *p, uint16_t addr)
{
    uint8_t val = mem_read(p, addr);
    uint8_t t = p->rY - val;
    set_rp(p, P_N, t >> 7);
    set_rp(p, P_C, p->rY >= val);
    set_rp(p, P_Z, t == 0);
}

void ins_dec(struct cpu_6502 *p, uint16_t addr)
{
    uint8_t val = mem_read(p, addr);
    val = (val - 1) & 0xFF;

    set_rp(p, P_N, val >> 7);
    set_rp(p, P_Z, val == 0);
    mem_write(p, addr, val);
}

void ins_dex(struct cpu_6502 *p, uint16_t addr)
{
    p->rX --;
    set_rp(p, P_Z, p->rX == 0);
    set_rp(p, P_N, p->rX >> 7);
}

void ins_dey(struct cpu_6502 *p, uint16_t addr)
{
    p->rY --;
    set_rp(p, P_Z, p->rY == 0);
    set_rp(p, P_N, p->rY >> 7);
}

void ins_eor(struct cpu_6502 *p, uint16_t addr)
{
    uint8_t val = mem_read(p, addr);
    p->rA ^= val;
    set_rp(p, P_Z, p->rA == 0);
    set_rp(p, P_N, p->rA >> 7);
}

void ins_inc(struct cpu_6502 *p, uint16_t addr)
{
    uint8_t val = mem_read(p, addr);
    val = (val + 1) & 0xFF;
    set_rp(p, P_Z, val == 0);
    set_rp(p, P_N, val >> 7);
    mem_write(p, addr, val);
}

void ins_inx(struct cpu_6502 *p, uint16_t addr)
{
    uint8_t val = p->rX;
    val = (val + 1) & 0xFF;
    set_rp(p, P_Z, val == 0);
    set_rp(p, P_N, val >> 7);
    p->rX = val;
}

void ins_iny(struct cpu_6502 *p, uint16_t addr)
{
    uint8_t val = p->rY;
    val = (val + 1) & 0xFF;
    set_rp(p, P_Z, val == 0);
    set_rp(p, P_N, val >> 7);
    p->rY = val;
}

void ins_jmp(struct cpu_6502 *p, uint16_t addr)
{
    p->rPC = addr;
}

void ins_jsr(struct cpu_6502 *p, uint16_t addr)
{
    uint16_t t = p->rPC - 1;
    stack_push(p, t >> 8);
    stack_push(p, t & 0xFF);
    p->rPC = addr;
}

void ins_lda(struct cpu_6502 *p, uint16_t addr)
{
    uint8_t val = mem_read(p, addr);
    p->rA = val;
    set_rp(p, P_Z, val == 0);
    set_rp(p, P_N, val >> 7);
}

void ins_ldx(struct cpu_6502 *p, uint16_t addr)
{
    uint8_t val = mem_read(p, addr);
    p->rX = val;
    set_rp(p, P_Z, val == 0);
    set_rp(p, P_N, val >> 7);
}

void ins_ldy(struct cpu_6502 *p, uint16_t addr)
{
    uint8_t val = mem_read(p, addr);
    p->rY = val;
    set_rp(p, P_Z, val == 0);
    set_rp(p, P_N, val >> 7);
}

void ins_lsra(struct cpu_6502 *p, uint16_t addr)
{
    uint8_t tmp = p->rA;

    set_rp(p, P_C, tmp & 0x01);
    tmp = (tmp >> 1) & 0x7F;
    set_rp(p, P_Z, tmp == 0);
    set_rp(p, P_N, tmp >> 7);

    p->rA = tmp;
}

void ins_lsr(struct cpu_6502 *p, uint16_t addr)
{
    uint8_t tmp = mem_read(p, addr);

    set_rp(p, P_C, tmp & 0x01);
    tmp = (tmp >> 1) & 0x7F;
    set_rp(p, P_Z, tmp == 0);
    set_rp(p, P_N, tmp >> 7);

    mem_write(p, addr, tmp);
}

void ins_nop(struct cpu_6502 *p, uint16_t addr)
{
    // 2EZ4NOP
}

void ins_ora(struct cpu_6502 *p, uint16_t addr)
{
    uint8_t val = mem_read(p, addr);
    p->rA |= val;
    set_rp(p, P_N, p->rA >> 7);
    set_rp(p, P_Z, p->rA == 0);
}

void ins_pha(struct cpu_6502 *p, uint16_t addr)
{
    stack_push(p, p->rA);
}

void ins_php(struct cpu_6502 *p, uint16_t addr)
{
    stack_push(p, read_rp(p) | 0x30);
}

void ins_pla(struct cpu_6502 *p, uint16_t addr)
{
    uint8_t val = stack_pop(p);
    p->rA = val;
    set_rp(p, P_N, val >> 7);
    set_rp(p, P_Z, val == 0);
}

void ins_plp(struct cpu_6502 *p, uint16_t addr)
{
    uint8_t val = stack_pop(p);
    write_rp(p, val);
    set_rp(p, P_B, 1);
}

void ins_rola(struct cpu_6502 *p, uint16_t addr)
{
    uint8_t val = p->rA;

    uint8_t t = (val >> 7);
    val = (val << 1) & 0xFE;
    val |= get_rp(p, P_C);
    set_rp(p, P_C, t);
    set_rp(p, P_N, val >> 7);
    set_rp(p, P_Z, val == 0);

    p->rA = val;
}

void ins_rol(struct cpu_6502 *p, uint16_t addr)
{
    uint8_t val = mem_read(p, addr);

    uint8_t t = (val >> 7);
    val = (val << 1) & 0xFE;
    val |= get_rp(p, P_C);
    set_rp(p, P_C, t);
    set_rp(p, P_N, val >> 7);
    set_rp(p, P_Z, val == 0);

    mem_write(p, addr, val);
}

void ins_rora(struct cpu_6502 *p, uint16_t addr)
{
    uint8_t val = p->rA;

    uint8_t t = (val & 0x01);
    val = (val >> 1) & 0x7F;
    val |= get_rp(p, P_C) ? 0x80 : 0x00;
    set_rp(p, P_C, t);
    set_rp(p, P_N, val >> 7);
    set_rp(p, P_Z, val == 0);

    p->rA = val;
}

void ins_ror(struct cpu_6502 *p, uint16_t addr)
{
    uint8_t val = mem_read(p, addr);

    uint8_t t = (val & 0x01);
    val = (val >> 1) & 0x7F;
    val |= get_rp(p, P_C) ? 0x80 : 0x00;
    set_rp(p, P_C, t);
    set_rp(p, P_N, val >> 7);
    set_rp(p, P_Z, val == 0);

    mem_write(p, addr, val);
}

void ins_rti(struct cpu_6502 *p, uint16_t addr)
{
    uint16_t pc;
    write_rp(p, stack_pop(p) | 0x10);
    pc = stack_pop(p);
    pc |= (stack_pop(p)) << 8;
    p->rPC = pc;
}

void ins_rts(struct cpu_6502 *p, uint16_t addr)
{
    uint16_t pc;
    pc = stack_pop(p);
    pc |= stack_pop(p) << 8;
    p->rPC = pc + 1;
}

void ins_sbc(struct cpu_6502 *p, uint16_t addr)
{
    uint8_t val = mem_read(p, addr);
    uint16_t t;

    t = p->rA - val - !get_rp(p, P_C);
    set_rp(p, P_V, (((p->rA ^ val) & (p->rA ^ t)) & 0x80) ? 1 : 0);

    set_rp(p, P_C, (t & 0xff00) == 0);

    p->rA = t & 0xFF;
    set_rp(p, P_N, p->rA >> 7);
    set_rp(p, P_Z, p->rA == 0);
}

void ins_sec(struct cpu_6502 *p, uint16_t addr)
{
    set_rp(p, P_C, 1);
}

void ins_sed(struct cpu_6502 *p, uint16_t addr)
{
    set_rp(p, P_D, 1);
}

void ins_sei(struct cpu_6502 *p, uint16_t addr)
{
    set_rp(p, P_I, 1);
}

void ins_sta(struct cpu_6502 *p, uint16_t addr)
{
    mem_write(p, addr, p->rA);
}

void ins_stx(struct cpu_6502 *p, uint16_t addr)
{
    mem_write(p, addr, p->rX);
}

void ins_sty(struct cpu_6502 *p, uint16_t addr)
{
    mem_write(p, addr, p->rY);
}

void ins_tax(struct cpu_6502 *p, uint16_t addr)
{
    p->rX = p->rA;
    set_rp(p, P_N, p->rX >> 7);
    set_rp(p, P_Z, p->rX == 0);
}

void ins_tay(struct cpu_6502 *p, uint16_t addr)
{
    p->rY = p->rA;
    set_rp(p, P_N, p->rY >> 7);
    set_rp(p, P_Z, p->rY == 0);
}

void ins_tsx(struct cpu_6502 *p, uint16_t addr)
{
    p->rX = p->rS;
    set_rp(p, P_N, p->rX >> 7);
    set_rp(p, P_Z, p->rX == 0);
}

void ins_txa(struct cpu_6502 *p, uint16_t addr)
{
    p->rA = p->rX;
    set_rp(p, P_N, p->rA >> 7);
    set_rp(p, P_Z, p->rA == 0);
}

void ins_txs(struct cpu_6502 *p, uint16_t addr)
{
    p->rS = p->rX;
}

void ins_tya(struct cpu_6502 *p, uint16_t addr)
{
    p->rA = p->rY;
    set_rp(p, P_N, p->rA >> 7);
    set_rp(p, P_Z, p->rA == 0);
}

void ins_aax(struct cpu_6502 *p, uint16_t addr)
{
    mem_write(p, addr, p->rA & p->rX);
}

void ins_lax(struct cpu_6502 *p, uint16_t addr)
{
    uint8_t val = mem_read(p, addr);
    p->rA = val;
    p->rX = val;
    set_rp(p, P_Z, val == 0);
    set_rp(p, P_N, val >> 7);
}

void ins_unknown(struct cpu_6502 *p, uint16_t addr)
{
    printf("Unknown instruction, emergency out\n");
//    exit(0);
}
