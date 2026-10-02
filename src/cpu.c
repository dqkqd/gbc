#include "cpu.h"
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
}

uint8_t reg_f(Reg *reg) {
  return ((uint8_t)reg->flags.z << 7) | ((uint8_t)reg->flags.n << 6) |
         ((uint8_t)reg->flags.h << 5) | ((uint8_t)reg->flags.c << 4);
}
