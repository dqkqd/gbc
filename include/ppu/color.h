#ifndef PPU_COLOR_H
#define PPU_COLOR_H

#include <stdint.h>

#include "constant.h"
#include "mem.h"

typedef struct {
  uint8_t data;
} Palette;

typedef struct {
  int index;
  Palette *p;
} ColorIndex;

typedef struct {
  uint8_t r, g, b;
} Color;

inline void color_get_palette_bgp(const Mem *mem, Palette *p) {
  p->data = mem_read_u8(mem, COLOR_PALETTE_BGP_ADDR);
}

inline void color_get_palette_obj0(const Mem *mem, Palette *p) {
  p->data = mem_read_u8(mem, COLOR_PALETTE_OBJ0_ADDR);
}

inline void color_get_palette_obj1(const Mem *mem, Palette *p) {
  p->data = mem_read_u8(mem, COLOR_PALETTE_OBJ1_ADDR);
}

static const Color color_mapping[4] = {
    {255, 255, 255},
    {170, 170, 170},
    {85, 85, 85},
    {0, 0, 0},
};

static const Color COLOR_WHITE = color_mapping[0];

static inline void color_from_index(const ColorIndex *ci, Color *c) {
  int index = (ci->p->data >> (ci->index * 2)) & 0x03;
  *c = color_mapping[index];
}

#endif // !PPU_COLOR_H
