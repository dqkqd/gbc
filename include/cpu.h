#ifndef CPU_H
#define CPU_H

#include <stdbool.h>
#include <stdint.h>

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
} Cpu;

void cpu_init(Cpu *cpu);

uint8_t reg_f(Reg *reg);

#endif // !CPU_H
