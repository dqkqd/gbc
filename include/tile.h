#ifndef TILE_H
#define TILE_H

#include <stddef.h>
#include <stdint.h>

#include "constant.h"
#include "mem.h"

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

[[nodiscard]] bool tile_color_at(const Tile *tile, int x, int y, Color *color);

#endif // !TILE_H
