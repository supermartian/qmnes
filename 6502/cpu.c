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
#include "ppu.h"
#include "ins.h"

// All the addressing functions return the address of the prospective memory.

/*
 * Immediate addressing.
 * */
inline uint16_t addr_imm8(struct cpu_6502 *p)
{
    return p->rPC++;
}

/*
 * Relative addressing.
 * */
inline uint16_t addr_rela(struct cpu_6502 *p)
{
    uint16_t tmp;
    uint16_t ret = mem_read(p, p->rPC);
    p->rPC++;
    if (ret & 0x80)
        ret -= 0x100;
    
    ret += p->rPC;
    // Relative addressing is only used by branch instructions.
    // Any page crossing issue will be dealt inside the instruction function.
    return ret;
}

/*
 * Absolute addressing.
 * */
inline uint16_t addr_abs(struct cpu_6502 *p)
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
inline uint16_t addr_zp(struct cpu_6502 *p)
{
    uint16_t ret = mem_read(p, p->rPC);
    p->rPC++;
    return ret;
}

/*
 * Absolute indexed by X addressing.
 * */
inline uint16_t addr_aix(struct cpu_6502 *p)
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
inline uint16_t addr_aiy(struct cpu_6502 *p)
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
inline uint16_t addr_zpix(struct cpu_6502 *p)
{
    uint16_t ret = mem_read(p, p->rPC);
    p->rPC++;
    return (ret + p->rX) & 0xff;
}

/*
 * Zero page indexed by Y addressing.
 * */
inline uint16_t addr_zpiy(struct cpu_6502 *p)
{
    uint16_t ret = mem_read(p, p->rPC);
    p->rPC++;
    return (ret + p->rY) & 0xff;
}

/*
 * Indexed indirect addressing.
 * */
inline uint16_t addr_idiri(struct cpu_6502 *p)
{
    uint16_t ret = mem_read(p, p->rPC);
    p->rPC++;
    ret += p->rX;
    ret &= 0xff;
    ret = mem_read(p, ret) | (mem_read(p, ret + 1) << 8);
    return ret;
}

/*
 * Indirect indexed addressing.
 * */
inline uint16_t addr_iidir(struct cpu_6502 *p)
{
    uint16_t ret;
    uint16_t tmp = mem_read(p, p->rPC++);
    ret = mem_read(p, tmp++);
    ret |= mem_read(p, tmp & 0xff) << 8;
    ret += p->rY;
    page_boundary_chk(p, ret);
    return ret;
}

/*
 * Absolute indirect addressing.
 * There's a known bug here. The address doesn't cross pages.
 * http://wiki.nesdev.com/w/index.php/Errata#CPU
 * */
inline uint16_t addr_absidir(struct cpu_6502 *p)
{
    uint16_t tmp = mem_read(p, p->rPC);
    uint16_t ret;
    p->rPC++;
    tmp |= mem_read(p, p->rPC) << 8;
    p->rPC++;
    ret = mem_read(p, tmp);
    tmp = (tmp & 0xff00) | ((tmp + 1) & 0xff);
    ret |= mem_read(p, tmp) << 8;
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
    p->ins_cnt = 1;

    start_addr = mem_read(p, 0xFFFC);
    start_addr |= mem_read(p, 0xFFFD) << 8;

    p->rPC = start_addr;
    printf("start from %x\n", start_addr);
    p->rS = 0xFD;
    p->rA = 0;
    p->rX = 0;
    p->rY = 0;

    write_rp(p, 0x24);

    p->nmi = 0;
    p->irq = 0;
    p->rst = 0;
}

uint8_t cpu_execute_op(struct cpu_6502 *p, uint8_t opcode)
{
    struct ins *i = &(ins_table[opcode]);
    uint16_t addr = 0;
    uint32_t oldcycle = p->cycle;
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
    p->ins_cnt++;
    return p->cycle - oldcycle;
}

void cpu_dump(struct cpu_6502 *p)
{
    printf("[%d]=====================================================\n", p->ins_cnt);
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
    printf("In cycle: %d\n", p->cycle * 3);
    printf("========================================================\n");
}

void cpu_run(struct cpu_6502 *p)
{
    uint8_t opcode;
    uint8_t op1;
    uint8_t op2;
    uint8_t cycle;
    for (;;)
    {
        opcode = mem_read(p, p->rPC);
        p->rPC++;
        cycle = cpu_execute_op(p, opcode);
        //cpu_dump(p);
        if (p->cycle >= 29658 && p->ppu->ppu_ready == 0) {
            p->ppu->ppu_ready = 1;
        }
        ppu_run(p->ppu, p, cycle * 3);
        if (p->nmi || p->irq) {
            cpu_handle_intr(p);
        }

    }
}

void joystick_write(struct cpu_6502 *p, uint8_t val)
{
    if ((val & 0x1)) {
        p->strobe = 0;
    } else if (p->strobe == 0 && !(val & 0x1)) {
        p->strobe = !p->strobe;
    }
}

uint8_t joystick_read(struct cpu_6502 *p)
{
    uint8_t ret;
    p->handle_input(p);
    if (p->strobe == 0) {
        p->keynow = 0;
    }

    //printf("p->keynow %d, p->key %d\n", p->keynow, p->keypad[p->keynow]);
    ret = p->keypad[p->keynow] ? 0x03 : 0x00; 
    p->keynow = (p->keynow + 1) % 8;
    return ret;
}

/*
 * Deal with NMI and IRQ.
 *
 */
void cpu_handle_intr(struct cpu_6502 *p)
{
    stack_push(p, p->rPC >> 8);
    stack_push(p, p->rPC & 0xFF);
    stack_push(p, read_rp(p));

    set_rp(p, P_I, 1);
    if (p->nmi) {
        p->rPC = mem_read(p, 0xFFFA) | ((uint16_t)mem_read(p, 0xFFFB) << 8);
        p->nmi = 0;
    } else {
        p->rPC = mem_read(p, 0xFFFE) | ((uint16_t)mem_read(p, 0xFFFF) << 8);
        p->irq = 0;
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
inline uint16_t mem_addr(uint16_t addr)
{
    if (addr < 0x2000) {
        return addr & 0x7FF;
    } else if (addr >= 0x2000 && addr < 0x4000) {
        return (addr & 0x7) | 0x2000;
    } else {
        return addr;
    }
}

/*
 * General memory reading.
 * */
inline uint8_t mem_read(struct cpu_6502 *p, uint16_t addr)
{
    uint16_t addr_ = mem_addr(addr);
    uint8_t ret = 0;
    if (addr_ < 0x0800) {
        ret = p->ram[addr_];
    } else if (addr_ >= 0x2000 && addr_ < 0x2008) {
        ret = ppu_read_reg(p->ppu, addr & 0x7);
    } else if (addr_ == 0x4016) {
        ret = joystick_read(p);
    } else if (addr_ >= 0x8000) {
        ret = p->rom_prg[addr_ & 0x7FFF];
    }
//    printf("memread %x-%x\n", addr, ret);
    return ret;
}

/*
 * General memory writing.
 * */
inline uint8_t mem_write(struct cpu_6502 *p, uint16_t addr, uint8_t val)
{
    uint16_t addr_ = mem_addr(addr);
    if (addr_ < 0x0800) {
        p->ram[addr_] = val;
    } else if (addr_ >= 0x2000 && addr_ < 0x2008) {
        ppu_write_reg(p->ppu, addr - 0x2000, val);
    } else if (addr_ == 0x4014) {
        ppu_dma(p->ppu, p, val);
    } else if (addr_ == 0x4016) {
        joystick_write(p, val);
    } else if (addr_ >= 0x8000) {
        //p->rom_prg[addr_ - 0x8000] = val;
    }
    return 0;
}

/*
 * Push a value into stack.
 * */
void stack_push(struct cpu_6502 *p, uint8_t val)
{
    mem_write(p, p->rS + STACK_START, val);
    p->rS--;
}

/*
 * Pop a value out of stack.
 * */
uint8_t stack_pop(struct cpu_6502 *p)
{
    p->rS++;
    uint8_t val = mem_read(p, p->rS + STACK_START);
    return val;
}

inline void set_rp(struct cpu_6502 *p, uint8_t bit, uint8_t val)
{
    p->rP = val ? (p->rP | (1 << bit)) : (p->rP & ~(1 << bit));
}

inline uint8_t get_rp(struct cpu_6502 *p, uint8_t bit)
{
    return (p->rP >> bit) & 0x1;
}

inline uint8_t read_rp(struct cpu_6502 *p)
{
    p->rP |= 0x20;
    p->rP &= ~(1 << P_B);
    return p->rP;
}

inline void write_rp(struct cpu_6502 *p, uint8_t val)
{
    p->rP = val;
}
