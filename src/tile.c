#include <stddef.h>
#include <stdint.h>

#include "constant.h"
#include "tile.h"

bool tile_color_at(const Tile *tile, int x, int y, Color *color) {
  const uint8_t *row_color_data = tile->data + ((ptrdiff_t)y * 2);
  uint8_t hi = (*row_color_data >> x) & 1;
  uint8_t lo = (*(row_color_data + 1) >> x) & 1;
  *color = color_mapping[(hi << 1) | lo];
  return true;
}
