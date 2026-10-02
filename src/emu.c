#include "emu.h"
#include "cpu.h"
#include "macro.h"
#include "mem.h"
#include "opcode.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int emu_init(Emu *emu, const char *rom) {
  FILE *f = fopen(rom, "rb");
  if (!f) {
    perror(rom);
    return 1;
  }

  cpu_init(&emu->cpu);
  mem_init(&emu->mem, f);
  fclose(f);

#if GB_TEST
  gb_doctor_log_init();
  atexit(gb_doctor_log_close);
#endif

  return 0;
}

void emu_destroy(Emu *emu) { mem_destroy(&emu->mem); }

uint8_t read_u8(Emu *emu) {
  uint8_t res = mem_read_u8(&emu->mem, emu->cpu.reg.pc);
  emu->cpu.reg.pc++;
  return res;
}

uint16_t read_u16(Emu *emu) {
  uint16_t res = mem_read_u16(&emu->mem, emu->cpu.reg.pc);
  emu->cpu.reg.pc += 2;
  return res;
}

void decode(Emu *emu, Opcode *opcode) {
  GAMEBOY_DOCTOR(emu);
  uint8_t kind = read_u8(emu);
  switch (kind) {
  case 0x00:
    opcode->kind = OPCODE_KIND_0x00_NOP;
    break;
  case 0xc3:
    opcode->kind = OPCODE_KIND_0xc3_JP_a16;
    opcode->a16 = read_u16(emu);
    break;
  default: {
    FATAL("Cannot decode opcode 0x(%x)\n", kind);
  }
  }
}

uint8_t execute(Emu *emu, const Opcode *const opcode) {
  switch (opcode->kind) {
  case OPCODE_KIND_0x00_NOP:
    return 4;
  case OPCODE_KIND_0xc3_JP_a16:
    emu->cpu.reg.pc = opcode->a16;
    return 16;
  default:
    FATAL("Cannot execute opcode 0x(%x)\n", (uint8_t)opcode->kind);
  }
  return 4;
}

void emu_loop(Emu *emu) {
  Opcode opcode;
  while (true) {
    decode(emu, &opcode);
    execute(emu, &opcode);
  }
}
