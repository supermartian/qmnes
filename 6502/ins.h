/*
 * ins.h
 * Copyright (C) 2014 martian <supermartian@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef __INS_H__
#define __INS_H__

#include "cpu.h"

#define IMPL_ADDR 0
#define ACCU_ADDR 1
#define RELA_ADDR 2
#define IMM8_ADDR 3
#define ABSO_ADDR 4
#define ZP_ADDR   5
#define AIX_ADDR  6
#define AIY_ADDR  7
#define ZPIX_ADDR 8
#define ZPIY_ADDR 9
#define IIDIR_ADDR 10
#define IDIRI_ADDR 11
#define ABSI_ADDR 12

#define INS_ADD(op, a, opname, c) ins_table[op].f = ins_##opname;ins_table[op].addr = a;ins_table[op].cycle = c

typedef void (*ins_func)(struct cpu_6502 *p, uint16_t addr);
struct ins {
    ins_func f;
    uint8_t addr;
    uint8_t cycle;
};
struct ins ins_table[0xFF];
void ins_table_init();

void ins_adc(struct cpu_6502 *p, uint16_t addr);
void ins_and(struct cpu_6502 *p, uint16_t addr);
void ins_asla(struct cpu_6502 *p, uint16_t addr);
void ins_asl(struct cpu_6502 *p, uint16_t addr);
void ins_bcc(struct cpu_6502 *p, uint16_t addr);
void ins_bcs(struct cpu_6502 *p, uint16_t addr);
void ins_beq(struct cpu_6502 *p, uint16_t addr);
void ins_bit(struct cpu_6502 *p, uint16_t addr);
void ins_bmi(struct cpu_6502 *p, uint16_t addr);
void ins_bne(struct cpu_6502 *p, uint16_t addr);
void ins_bpl(struct cpu_6502 *p, uint16_t addr);
void ins_brk(struct cpu_6502 *p, uint16_t addr);
void ins_bvc(struct cpu_6502 *p, uint16_t addr);
void ins_bvs(struct cpu_6502 *p, uint16_t addr);
void ins_clc(struct cpu_6502 *p, uint16_t addr);
void ins_cld(struct cpu_6502 *p, uint16_t addr);
void ins_cli(struct cpu_6502 *p, uint16_t addr);
void ins_clv(struct cpu_6502 *p, uint16_t addr);
void ins_cmp(struct cpu_6502 *p, uint16_t addr);
void ins_cpx(struct cpu_6502 *p, uint16_t addr);
void ins_cpy(struct cpu_6502 *p, uint16_t addr);
void ins_dec(struct cpu_6502 *p, uint16_t addr);
void ins_dex(struct cpu_6502 *p, uint16_t addr);
void ins_dey(struct cpu_6502 *p, uint16_t addr);
void ins_eor(struct cpu_6502 *p, uint16_t addr);
void ins_inc(struct cpu_6502 *p, uint16_t addr);
void ins_inx(struct cpu_6502 *p, uint16_t addr);
void ins_iny(struct cpu_6502 *p, uint16_t addr);
void ins_jmp(struct cpu_6502 *p, uint16_t addr);
void ins_jsr(struct cpu_6502 *p, uint16_t addr);
void ins_lda(struct cpu_6502 *p, uint16_t addr);
void ins_ldx(struct cpu_6502 *p, uint16_t addr);
void ins_ldy(struct cpu_6502 *p, uint16_t addr);
void ins_lsra(struct cpu_6502 *p, uint16_t addr);
void ins_lsr(struct cpu_6502 *p, uint16_t addr);
void ins_nop(struct cpu_6502 *p, uint16_t addr);
void ins_ora(struct cpu_6502 *p, uint16_t addr);
void ins_pha(struct cpu_6502 *p, uint16_t addr);
void ins_php(struct cpu_6502 *p, uint16_t addr);
void ins_pla(struct cpu_6502 *p, uint16_t addr);
void ins_plp(struct cpu_6502 *p, uint16_t addr);
void ins_rola(struct cpu_6502 *p, uint16_t addr);
void ins_rol(struct cpu_6502 *p, uint16_t addr);
void ins_rora(struct cpu_6502 *p, uint16_t addr);
void ins_ror(struct cpu_6502 *p, uint16_t addr);
void ins_rti(struct cpu_6502 *p, uint16_t addr);
void ins_rts(struct cpu_6502 *p, uint16_t addr);
void ins_sbc(struct cpu_6502 *p, uint16_t addr);
void ins_sec(struct cpu_6502 *p, uint16_t addr);
void ins_sed(struct cpu_6502 *p, uint16_t addr);
void ins_sei(struct cpu_6502 *p, uint16_t addr);
void ins_sta(struct cpu_6502 *p, uint16_t addr);
void ins_stx(struct cpu_6502 *p, uint16_t addr);
void ins_sty(struct cpu_6502 *p, uint16_t addr);
void ins_tax(struct cpu_6502 *p, uint16_t addr);
void ins_tay(struct cpu_6502 *p, uint16_t addr);
void ins_tsx(struct cpu_6502 *p, uint16_t addr);
void ins_txa(struct cpu_6502 *p, uint16_t addr);
void ins_txs(struct cpu_6502 *p, uint16_t addr);
void ins_tya(struct cpu_6502 *p, uint16_t addr);

#endif /* !__INS_H__ */
