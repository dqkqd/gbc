#ifndef MEM_H
#define MEM_H

#include <stdint.h>
#include <stdio.h>

typedef struct {
  uint8_t *rom;
  uint8_t *ram;
} Mem;

void mem_init(Mem *mem, FILE *f);
void mem_destroy(Mem *mem);

uint8_t mem_read_u8(const Mem *mem, uint16_t addr);
uint16_t mem_read_u16(const Mem *mem, uint16_t addr);

#endif // !MEM_H
