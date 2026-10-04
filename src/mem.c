#include "mem.h"
#include "constant.h"
#include "macro.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void mem_init(Mem *const mem, FILE *f) {
  fseek(f, 0, SEEK_END);
  const long size = ftell(f);
  mem->rom = malloc(size * sizeof(uint8_t));
  fseek(f, 0, SEEK_SET);
  if (!fread(mem->rom, sizeof(uint8_t), size, f)) {
    FATAL("Cannot read rom file");
  }

  mem->ram = malloc(0x10000 * sizeof(uint8_t));
  mem->cycles = 0;
}

void mem_destroy(Mem *const mem) {
  free(mem->rom);
  free(mem->ram);
}

uint8_t mem_read_u8(const Mem *const mem, uint16_t addr) {
#ifdef GB_TEST
  if (addr == 0xFF44) {
    return 0x90;
  }
#endif
  return *mem_ref_u8(mem, addr);
}

uint8_t *mem_ref_u8(const Mem *const mem, uint16_t addr) {
  if (addr < 0x7fff) {
    return &mem->rom[addr];
  }
  return &mem->ram[addr];
}

uint16_t mem_read_u16(const Mem *const mem, uint16_t addr) {
  uint16_t lo = mem_read_u8(mem, addr);
  uint16_t hi = mem_read_u8(mem, addr + 1);
  return (hi << 8U) | lo;
}

void mem_write_u8(Mem *const mem, uint16_t addr, uint8_t byte) {
  if (addr < 0x7fff) {
    FATAL("writing to rom at 0x%04X", addr);
  }
  mem->ram[addr] = byte;
}

void mem_write_u16(Mem *mem, uint16_t addr, uint16_t byte) {
  if (addr < 0x7fff) {
    FATAL("writing to rom at 0x%04X", addr);
  }
  mem->ram[addr] = byte;          // lo
  mem->ram[addr + 1] = byte >> 8; // hi
}

void mem_tick(Mem *mem, uint8_t t_state_cycle) {
  if (t_state_cycle == 0) {
    return;
  }

  //  https://gbdev.io/pandocs/Timer_and_Divider_Registers.html#ff05--tima-timer-counter
  uint8_t tac = mem_read_u8(mem, TAC_ADDR);
  bool enabled = (tac & 0x04) != 0;
  if (!enabled) {
    return;
  }

  static const int clocks[4] = {256, 4, 16, 64};
  uint16_t clock = clocks[tac & 0x03];

  mem->cycles += t_state_cycle >> 2;

  // lower than clock boundary
  if (mem->cycles < clock) {
    return;
  }

  uint8_t inc = mem->cycles / clock;
  mem->cycles %= clock;

  uint8_t *tima = mem_ref_u8(mem, TIMA_ADDR);
  // adding would not cause overflow
  if (inc <= 0xFF - *tima) {
    *tima += inc;
    return;
  }

  // overflow case, enable interrupt
  uint8_t *if_ = mem_ref_u8(mem, IF_ADDR);
  *if_ |= I_TIMER_FLAG;

  // set tima to the correct value
  uint8_t tma = mem_read_u8(mem, TMA_ADDR);
  *tima = tma + ((inc - (0xFF - *tima)) % (0xFF - tma));
}
