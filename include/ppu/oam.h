#ifndef PPU_OAM_H
#define PPU_OAM_H

#include <stdint.h>

#include "constant.h"
#include "mem.h"
#include "tile.h"

typedef struct {
  int oam_index; // the position in the oam index, for sort tiebreaker
  int off_y;     // the relative offset y according to the tile location
  int x;         // the actual x on the screen
  uint8_t flags; // the object flags
  Tile tile;     // the tile data
} Object;

typedef struct {
  int n_objects;
  Object object_buffer[OAM_MAX_OBJECTS];
} PpuOAMScanResult;

/**
 * Where the current object at `*oam_ptr` can be cut by the `scanning_line`,
 * if it is, then load it to `*obj`
 */
[[nodiscard]]
bool oam_get_obj(const Mem *mem, const uint8_t *oam_ptr, Object *obj,
                 int oam_index, int scanning_line, int object_size);

void oam_sort(Object *obj, int n);

#endif // !PPU_OAM_H
