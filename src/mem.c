#include "mem.h"
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
