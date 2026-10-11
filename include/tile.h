#ifndef TILE_H
#define TILE_H

#include <stddef.h>
#include <stdint.h>

#include "constant.h"
#include "mem.h"
#include "ppu/color.h"

typedef struct {
  const uint8_t *data;
} Tile;

/*
 * TileData fetch 0x8000 method
 * */
inline void get_tile_data_0x8000(const Mem *mem, uint8_t tile_index,
                                 Tile *tile) {
  tile->data = mem_ref_u8(mem, TILE_BLOCK0_ADDR + (tile_index * TILE_SIZE));
}

/*
 * TileData fetch for background and window
 * */
inline void get_tile_data_0x8800(const Mem *mem, int8_t tile_index,
                                 Tile *tile) {
  tile->data =
      mem_ref_u8(mem, TILE_BLOCK1_ADDR + ((int)tile_index * TILE_SIZE));
}

inline ColorIndex tile_color_index(const Tile *tile, int x, int y, Palette *p) {
  const uint8_t *row_color_data = tile->data + ((ptrdiff_t)y * 2);
  // the left most bit contains the color for the smallest x. We need to revert
  // x.
  int xpos = 7 - x;
  int lo = (*row_color_data >> xpos) & 1;
  int hi = (*(row_color_data + 1) >> xpos) & 1;
  return (ColorIndex){.index = (hi << 1) | lo, p};
}

#endif // !TILE_H
