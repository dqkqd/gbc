#include "ppu/oam.h"

bool oam_get_obj(const Mem *mem, const uint8_t *oam_ptr, Object *obj,
                 int oam_index, int scanning_line, int object_size) {
  obj->oam_index = oam_index;

  // Byte 0: Y position
  int y = *oam_ptr;

  // For object_size = 8:
  // then the object is loaded from `[y - 16, y - 8)`
  // For object_size = 16:
  // then the object is loaded from `[y - 16, y)`
  int lower_bound = y - 16;
  int upper_bound = object_size == 16 ? y : y - 8;
  if (scanning_line < lower_bound || upper_bound <= scanning_line) {
    return false;
  }

  // Byte 1: X position
  obj->x = *(oam_ptr + 1);

  // Byte 2: tile index
  uint8_t tile_index = *(oam_ptr + 2);

  uint8_t tile_index1 = (tile_index & 0xFE);
  uint8_t tile_index2 = (tile_index & 0xFE) | 1;

  // Byte 3: objibute flags
  obj->flags = *(oam_ptr + 3);

  // the actual offset y
  int off_y = scanning_line - lower_bound;
  int y_flipped = (obj->flags & OBJ_Y_FLIP_FLAG) != 0;

  // get the actual tile data and refine the off_y to that tile
  if (object_size == 8) {
    // the data is always in the first half, nothing to worry here
    get_tile_data_0x8000(mem, tile_index1, &obj->tile);
    obj->off_y = y_flipped ? 7 - off_y : off_y;
  } else {
    // the data can be in the first half or the second half, depends on the
    // offset value and the flip condition

    if (off_y < 8 && !y_flipped) {
      // trivial: first half
      obj->off_y = off_y;
      get_tile_data_0x8000(mem, tile_index1, &obj->tile);
    } else if (off_y < 8 && y_flipped) {
      // first half, but flipped to the second half
      obj->off_y = 7 - off_y;
      get_tile_data_0x8000(mem, tile_index2, &obj->tile);
    } else if (off_y >= 8 && !y_flipped) {
      // trivial: second half
      obj->off_y = off_y - 8;
      get_tile_data_0x8000(mem, tile_index2, &obj->tile);
    } else {
      // second half but flipped to the first half
      obj->off_y = 15 - off_y;
      get_tile_data_0x8000(mem, tile_index1, &obj->tile);
    }
  }

  return true;
}

int partition(Object *obj, int lo, int hi) {
  Object *pivot = &obj[hi];
  int i = lo;

  Object tmp;
  Object *cur;
  for (cur = obj + lo; cur < obj + hi; cur++) {
    if (cur->x < pivot->x ||
        (cur->x == pivot->x && cur->oam_index < pivot->oam_index)) {
      // swap cur with obj[i];
      tmp = *cur;
      *cur = obj[i];
      obj[i] = tmp;
      i++;
    }
  }

  // swap pivot
  tmp = *pivot;
  *pivot = obj[i];
  obj[i] = tmp;
  return i;
}

void quicksort(Object *obj, int lo, int hi) { // NOLINT
  if (lo >= hi) {
    return;
  }
  int p = partition(obj, lo, hi);
  quicksort(obj, lo, p - 1);
  quicksort(obj, p + 1, hi);
}

void oam_sort(Object *obj, int n) { quicksort(obj, 0, n - 1); }
