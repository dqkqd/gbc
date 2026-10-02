#include "mem.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void mem_init(Mem *mem, FILE *f) {
  fseek(f, 0, SEEK_END);
  const long size = ftell(f);
  mem->rom = malloc(size * sizeof(uint8_t));
  fseek(f, 0, SEEK_SET);
  fread(mem->rom, sizeof(uint8_t), size, f);

  mem->ram = malloc(0x10000 * sizeof(uint8_t));
}

void mem_destroy(Mem *mem) {
  free(mem->rom);
  free(mem->ram);
}

uint8_t mem_read_u8(const Mem *mem, uint16_t addr) {
  if (addr < 0x7fff) {
    return mem->rom[addr];
  }
  return mem->ram[addr];
}

uint16_t mem_read_u16(const Mem *mem, uint16_t addr) {
  uint16_t lo = mem_read_u8(mem, addr);
  uint16_t hi = mem_read_u8(mem, addr + 1);
  return (hi << 8U) | lo;
}
