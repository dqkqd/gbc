#ifndef MEM_H
#define MEM_H

#include <stdint.h>
#include <stdio.h>

typedef struct {
  uint8_t *rom;
  uint8_t *ram;
  uint16_t cycles;
} Mem;

void mem_init(Mem *mem, FILE *f);
void mem_destroy(Mem *mem);

uint8_t mem_read_u8(const Mem *mem, uint16_t addr);
uint8_t *mem_ref_u8(const Mem *mem, uint16_t addr);
uint16_t mem_read_u16(const Mem *mem, uint16_t addr);

void mem_write_u8(Mem *mem, uint16_t addr, uint8_t byte);
void mem_write_u16(Mem *mem, uint16_t addr, uint16_t byte);

void mem_tick(Mem *mem, uint8_t t_state_cycle);

#endif // !MEM_H
