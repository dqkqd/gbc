#ifndef CPU_H
#define CPU_H

#include <stdbool.h>
#include <stdint.h>

#include "mem.h"

typedef struct {
  bool z, n, h, c;
} RegFlags;

typedef struct {
  uint8_t b, c, d, e, h, l, a;
  uint16_t sp, pc;
  RegFlags flags;
} Reg;

typedef struct {
  Reg reg;
  bool ime;

  bool halted;
  bool halt_bug;

  // The effect of ei is delayed by one instruction
  bool ei_waiting;
} Cpu;

void cpu_init(Cpu *cpu);

uint8_t reg_f(const Reg *reg);
uint16_t reg_bc(const Reg *reg);
uint16_t reg_de(const Reg *reg);
uint16_t reg_hl(const Reg *reg);
uint16_t reg_af(const Reg *reg);

void reg_set_bc(Reg *reg, uint16_t bc);
void reg_set_de(Reg *reg, uint16_t de);
void reg_set_hl(Reg *reg, uint16_t hl);
void reg_set_af(Reg *reg, uint16_t af);

void cpu_inc_r8(Cpu *cpu, uint8_t *r8);
void cpu_dec_r8(Cpu *cpu, uint8_t *r8);

void cpu_push(Cpu *cpu, Mem *mem, uint16_t addr);
uint16_t cpu_pop(Cpu *cpu, Mem *mem);
void cpu_call(Cpu *cpu, Mem *mem, uint16_t addr);
void cpu_ret(Cpu *cpu, Mem *mem);

void cpu_add(Cpu *cpu, uint8_t v);
void cpu_adc(Cpu *cpu, uint8_t v);
void cpu_sub(Cpu *cpu, uint8_t v);
void cpu_sbc(Cpu *cpu, uint8_t v);
void cpu_and(Cpu *cpu, uint8_t v);
void cpu_xor(Cpu *cpu, uint8_t v);
void cpu_or(Cpu *cpu, uint8_t v);
void cpu_cp(Cpu *cpu, uint8_t v);

void cpu_rlca(Cpu *cpu);
void cpu_rrca(Cpu *cpu);
void cpu_rla(Cpu *cpu);
void cpu_rra(Cpu *cpu);

void cpu_rlc(Cpu *cpu, uint8_t *v);
void cpu_rrc(Cpu *cpu, uint8_t *v);
void cpu_rl(Cpu *cpu, uint8_t *v);
void cpu_rr(Cpu *cpu, uint8_t *v);
void cpu_sla(Cpu *cpu, uint8_t *v);
void cpu_sra(Cpu *cpu, uint8_t *v);
void cpu_srl(Cpu *cpu, uint8_t *v);
void cpu_swap(Cpu *cpu, uint8_t *v);
void cpu_bit(uint8_t n, Cpu *cpu, uint8_t v);
void cpu_res(uint8_t n, uint8_t *v);
void cpu_set(uint8_t n, uint8_t *v);

void cpu_add_hl(Cpu *cpu, uint16_t v);
void cpu_daa(Cpu *cpu);

void cpu_add_sp_e8(Cpu *cpu, int8_t v);
void cpu_ld_hl_sp_e8(Cpu *cpu, int8_t v);

void cpu_cpl(Cpu *cpu);
void cpu_ccf(Cpu *cpu);
void cpu_scf(Cpu *cpu);

uint8_t cpu_interrupt(Cpu *cpu, Mem *mem);

bool cpu_has_interrupt_pending(Mem *mem);

#endif // !CPU_H
