#include "cpu.h"
#include "macro.h"
#include "mem.h"
#include <stddef.h>
#include <stdint.h>

void cpu_init(Cpu *cpu) {
  cpu->reg.a = 0x01;
  cpu->reg.b = 0x00;
  cpu->reg.c = 0x13;
  cpu->reg.d = 0x00;
  cpu->reg.e = 0xD8;
  cpu->reg.h = 0x01;
  cpu->reg.l = 0x4D;
  cpu->reg.flags.z = true;
  cpu->reg.flags.n = false;
  cpu->reg.flags.h = true;
  cpu->reg.flags.c = true;
  cpu->reg.sp = 0XFFFE;
  cpu->reg.pc = 0x0100;
  cpu->ime = false;
}

uint8_t reg_f(const Reg *const reg) {
  return ((uint8_t)reg->flags.z << 7) | ((uint8_t)reg->flags.n << 6) |
         ((uint8_t)reg->flags.h << 5) | ((uint8_t)reg->flags.c << 4);
}

uint16_t reg_bc(const Reg *const reg) {
  return ((uint16_t)reg->b << 8) | reg->c;
}
uint16_t reg_de(const Reg *const reg) {
  return ((uint16_t)reg->d << 8) | reg->e;
}
uint16_t reg_hl(const Reg *const reg) {
  return ((uint16_t)reg->h << 8) | reg->l;
}
uint16_t reg_af(const Reg *const reg) {
  return ((uint16_t)reg->a << 8) | reg_f(reg);
}

static void reg_set_f(Reg *const reg, uint8_t f) {
  reg->flags.z = (((f >> 7) & 1) != 0);
  reg->flags.n = (((f >> 6) & 1) != 0);
  reg->flags.h = (((f >> 5) & 1) != 0);
  reg->flags.c = (((f >> 4) & 1) != 0);
}

void reg_set_bc(Reg *const reg, uint16_t bc) {
  reg->b = bc >> 8;
  reg->c = bc;
}
void reg_set_de(Reg *const reg, uint16_t de) {
  reg->d = de >> 8;
  reg->e = de;
}
void reg_set_hl(Reg *const reg, uint16_t hl) {
  reg->h = hl >> 8;
  reg->l = hl;
}
void reg_set_af(Reg *const reg, uint16_t af) {
  reg->a = af >> 8;
  reg_set_f(reg, af);
}

void cpu_inc_r8(Cpu *cpu, uint8_t *const r8) {
  uint8_t res = *r8 + 1;
  cpu->reg.flags.z = res == 0;
  cpu->reg.flags.n = false;
  cpu->reg.flags.h = (*r8 & 0x0F) == 0x0F;
  *r8 = res;
}

void cpu_dec_r8(Cpu *cpu, uint8_t *const r8) {
  uint8_t res = *r8 - 1;
  cpu->reg.flags.z = res == 0;
  cpu->reg.flags.n = true;
  cpu->reg.flags.h = (*r8 & 0x0F) == 0;
  *r8 = res;
}

void cpu_push(Cpu *const cpu, Mem *const mem, uint16_t addr) {
  if (cpu->reg.sp < 2) {
    FATAL("stack overflow");
  }
  cpu->reg.sp -= 2;
  mem_write_u16(mem, cpu->reg.sp, addr);
}

uint16_t cpu_pop(Cpu *cpu, Mem *mem) {
  uint16_t res = mem_read_u16(mem, cpu->reg.sp);
  cpu->reg.sp += 2;
  return res;
}

void cpu_call(Cpu *const cpu, Mem *const mem, uint16_t addr) {
  cpu_push(cpu, mem, cpu->reg.pc);
  cpu->reg.pc = addr;
}

void cpu_ret(Cpu *cpu, Mem *mem) { cpu->reg.pc = cpu_pop(cpu, mem); }

void cpu_add(Cpu *cpu, uint8_t v) {
  uint8_t res = cpu->reg.a + v;
  cpu->reg.flags.z = res == 0;
  cpu->reg.flags.n = false;
  cpu->reg.flags.h = (res & 0xF) < (cpu->reg.a & 0xF);
  cpu->reg.flags.c = res < cpu->reg.a;
  cpu->reg.a = res;
}

void cpu_adc(Cpu *cpu, uint8_t v) {
  uint8_t rem = (uint8_t)cpu->reg.flags.c;
  uint16_t res16 = (uint16_t)cpu->reg.a + v + rem;
  uint8_t res = res16;
  cpu->reg.flags.z = res == 0;
  cpu->reg.flags.n = false;
  cpu->reg.flags.h = (cpu->reg.a & 0xF) + (v & 0xF) + rem > 0xF;
  cpu->reg.flags.c = res16 > 0xFF;
  cpu->reg.a = res;
}

void cpu_sub(Cpu *cpu, uint8_t v) {
  uint8_t res = cpu->reg.a - v;
  cpu->reg.flags.z = res == 0;
  cpu->reg.flags.n = true;
  cpu->reg.flags.h = (cpu->reg.a & 0xF) < (v & 0xF);
  cpu->reg.flags.c = cpu->reg.a < v;
  cpu->reg.a = res;
}

void cpu_sbc(Cpu *cpu, uint8_t v) {
  uint8_t rem = (uint8_t)cpu->reg.flags.c;
  uint16_t sum = (uint16_t)v + rem;
  uint8_t res = cpu->reg.a - sum;
  cpu->reg.flags.z = res == 0;
  cpu->reg.flags.n = true;
  cpu->reg.flags.h = (cpu->reg.a & 0xF) < ((v & 0xF) + rem);
  cpu->reg.flags.c = cpu->reg.a < sum;
  cpu->reg.a = res;
}

void cpu_and(Cpu *cpu, uint8_t v) {
  cpu->reg.a &= v;
  cpu->reg.flags.z = cpu->reg.a == 0;
  cpu->reg.flags.n = false;
  cpu->reg.flags.h = true;
  cpu->reg.flags.c = false;
}

void cpu_xor(Cpu *cpu, uint8_t v) {
  cpu->reg.a ^= v;
  cpu->reg.flags.z = cpu->reg.a == 0;
  cpu->reg.flags.n = false;
  cpu->reg.flags.h = false;
  cpu->reg.flags.c = false;
}

void cpu_or(Cpu *cpu, uint8_t v) {
  cpu->reg.a |= v;
  cpu->reg.flags.z = cpu->reg.a == 0;
  cpu->reg.flags.n = false;
  cpu->reg.flags.h = false;
  cpu->reg.flags.c = false;
}

void cpu_cp(Cpu *cpu, uint8_t v) {
  cpu->reg.flags.z = cpu->reg.a == v;
  cpu->reg.flags.n = true;
  cpu->reg.flags.h = (cpu->reg.a & 0xF) < (v & 0xF);
  cpu->reg.flags.c = cpu->reg.a < v;
}

void cpu_rlca(Cpu *cpu) {
  cpu->reg.a = (cpu->reg.a << 1) | (cpu->reg.a >> 7);
  cpu->reg.flags.z = false;
  cpu->reg.flags.n = false;
  cpu->reg.flags.h = false;
  cpu->reg.flags.c = (cpu->reg.a & 1) != 0;
}

void cpu_rrca(Cpu *cpu) {
  cpu->reg.flags.c = ((cpu->reg.a & 1) != 0);
  cpu->reg.a = (cpu->reg.a >> 1) | ((cpu->reg.a & 1) << 7);
  cpu->reg.flags.z = false;
  cpu->reg.flags.n = false;
  cpu->reg.flags.h = false;
}

void cpu_rla(Cpu *cpu) {
  bool c = (cpu->reg.a >> 7) != 0;
  cpu->reg.a = (cpu->reg.a << 1) | (uint8_t)cpu->reg.flags.c;
  cpu->reg.flags.z = false;
  cpu->reg.flags.n = false;
  cpu->reg.flags.h = false;
  cpu->reg.flags.c = c;
}

void cpu_rra(Cpu *cpu) {
  bool c = (cpu->reg.a & 1) != 0;
  cpu->reg.a = (cpu->reg.a >> 1) | ((uint8_t)cpu->reg.flags.c << 7);
  cpu->reg.flags.z = false;
  cpu->reg.flags.n = false;
  cpu->reg.flags.h = false;
  cpu->reg.flags.c = c;
}

void cpu_rlc(Cpu *cpu, uint8_t *v) {
  *v = (*v << 1) | (*v >> 7);
  cpu->reg.flags.z = *v == 0;
  cpu->reg.flags.n = false;
  cpu->reg.flags.h = false;
  cpu->reg.flags.c = ((*v & 1) != 0);
}

void cpu_rrc(Cpu *cpu, uint8_t *v) {
  cpu->reg.flags.c = ((*v & 1) != 0);
  *v = (*v >> 1) | ((*v & 1) << 7);
  cpu->reg.flags.z = *v == 0;
  cpu->reg.flags.n = false;
  cpu->reg.flags.h = false;
}

void cpu_rl(Cpu *cpu, uint8_t *v) {
  bool c = (*v >> 7) != 0;
  *v = (*v << 1) | (uint8_t)cpu->reg.flags.c;
  cpu->reg.flags.z = *v == 0;
  cpu->reg.flags.n = false;
  cpu->reg.flags.h = false;
  cpu->reg.flags.c = c;
}

void cpu_rr(Cpu *cpu, uint8_t *v) {
  bool c = (*v & 1) != 0;
  *v = (*v >> 1) | ((uint8_t)cpu->reg.flags.c << 7);
  cpu->reg.flags.z = *v == 0;
  cpu->reg.flags.n = false;
  cpu->reg.flags.h = false;
  cpu->reg.flags.c = c;
}

void cpu_sla(Cpu *cpu, uint8_t *v) {
  cpu->reg.flags.c = (*v >> 7) != 0;
  *v <<= 1;
  cpu->reg.flags.z = *v == 0;
  cpu->reg.flags.n = false;
  cpu->reg.flags.h = false;
}

void cpu_sra(Cpu *cpu, uint8_t *v) {
  cpu->reg.flags.c = (*v & 1) != 0;
  *v = (*v >> 1) | (*v & 0x70);
  cpu->reg.flags.z = *v == 0;
  cpu->reg.flags.n = false;
  cpu->reg.flags.h = false;
}

void cpu_srl(Cpu *cpu, uint8_t *v) {
  cpu->reg.flags.c = (*v & 1) != 0;
  *v >>= 1;
  cpu->reg.flags.z = *v == 0;
  cpu->reg.flags.n = false;
  cpu->reg.flags.h = false;
}

void cpu_swap(Cpu *cpu, uint8_t *v) {
  *v = (*v >> 8) | (*v << 8);
  cpu->reg.flags.z = *v == 0;
  cpu->reg.flags.n = false;
  cpu->reg.flags.h = false;
  cpu->reg.flags.c = false;
}

void cpu_bit(uint8_t n, Cpu *cpu, uint8_t v) {
  cpu->reg.flags.z = ((v >> n) & 1) != 0;
  cpu->reg.flags.n = false;
  cpu->reg.flags.h = true;
}

void cpu_res(uint8_t n, uint8_t *v) {
  uint8_t mask = ~1 << n;
  *v = *v & (~mask);
}

void cpu_set(uint8_t n, uint8_t *v) {
  uint8_t mask = 1 << n;
  *v = *v | mask;
}

void cpu_add_hl(Cpu *cpu, uint16_t v) {
  uint16_t hl = reg_hl(&cpu->reg);
  uint16_t res = hl + v;
  cpu->reg.flags.n = false;
  cpu->reg.flags.h = (res & 0x0FFF) < (hl & 0x0FFF);
  cpu->reg.flags.c = res < hl;
  reg_set_hl(&cpu->reg, res);
}

void cpu_daa(Cpu *cpu) {
  uint8_t adjust = 0;
  if (cpu->reg.flags.n) {
    if (cpu->reg.flags.h) {
      adjust += 0x06;
    }
    if (cpu->reg.flags.c) {
      adjust += 0x60;
    }
    cpu->reg.a -= adjust;
  } else {
    if ((int)cpu->reg.flags.h || ((cpu->reg.a & 0x0F) > 0x09)) {
      adjust += 0x06;
    }
    if ((int)cpu->reg.flags.c || (cpu->reg.a > 0x99)) {
      adjust += 0x60;
      cpu->reg.flags.c = true;
    }
    cpu->reg.a += adjust;
  }

  cpu->reg.flags.z = cpu->reg.a == 0;
  cpu->reg.flags.h = false;
}
