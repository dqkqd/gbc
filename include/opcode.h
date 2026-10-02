#ifndef OPCODE_H
#define OPCODE_H

#include <stdint.h>

typedef struct {
  enum {
    OPCODE_KIND_0x00_NOP = 0x00,
    OPCODE_KIND_0xc3_JP_a16 = 0xc3,
  } kind;

  uint8_t kind_byte;

  union {
    uint16_t n16;
    uint16_t a16;
  };
} Opcode;

#endif // !OPCODE_H
