#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "SDL3/SDL_init.h"
#include "constant.h"
#include "macro.h"
#include "mem.h"
#include "ppu.h"
#include "ppu/sdl.h"
#include "tile.h"

void ppu_init(Ppu *ppu, Mem *mem) {
  if (sdl_init(&ppu->sdl) == SDL_APP_FAILURE) {
    exit(1);
  };
  ppu->running = true;
  ppu->mem = mem;
  ppu->mode = 2;
  ppu->scanline = 0;
  ppu->win_y_condition = false;
}

void ppu_destroy(Ppu *ppu) { sdl_quit(&ppu->sdl); }

bool ppu_step(Ppu *ppu) {
  uint8_t *stat = mem_ref_u8(ppu->mem, LCD_STAT_ADDR);
  uint8_t *ly = mem_ref_u8(ppu->mem, LCD_LY_ADDR);
  uint8_t lyc = mem_read_u8(ppu->mem, LCD_LYC_ADDR);

  *ly = ppu->scanline;

  if (*ly == lyc) {
    *stat |= LCD_STAT_LYC_EQ_LY_FLAG;
  } else {
    *stat &= ~LCD_STAT_LYC_EQ_LY_FLAG;
  }

  int wy = mem_read_u8(ppu->mem, LCD_WIN_WY_ADDR);
  if (*ly == wy) {
    ppu->win_y_condition = true;
  }

  *stat = (*stat & 0xFC) | (ppu->mode & 0x03);

  DEBUG("PPU Mode %d, scanline %d", ppu->mode, ppu->scanline);

  // check interrupt
  if (((*stat & LCD_STAT_MODE_0_SELECT_FLAG) != 0 && ppu->mode == 0) ||
      ((*stat & LCD_STAT_MODE_1_SELECT_FLAG) != 0 && ppu->mode == 1) ||
      ((*stat & LCD_STAT_MODE_2_SELECT_FLAG) != 0 && ppu->mode == 2) ||
      ((*stat & LCD_STAT_LYC_SELECT_FLAG) != 0 &&
       (*stat & LCD_STAT_LYC_EQ_LY_FLAG) != 0)) {
    uint8_t *if_ = mem_ref_u8(ppu->mem, IF_ADDR);
    *if_ |= I_LCD_FLAG;
  }

  switch (ppu->mode) {
  case 2: {
    ppu_oam_scan(ppu);
    ppu->mode = 3;
    break;
  }
  case 3: {
    ppu_draw_line_pixels(ppu);
    ppu->mode = 0;
    break;
  }
  case 0: {
    if (ppu->scanline == 143) {
      // vertical blank, set it once!
      uint8_t *if_ = mem_ref_u8(ppu->mem, IF_ADDR);
      *if_ |= I_VBLANK_FLAG;
      ppu->win_y_condition = false;
      ppu->mode = 1;
    } else {
      ppu->mode = 2;
    }
    ppu->scanline++;
    break;
  }
  case 1: {
    ppu->scanline++;
    if (ppu->scanline > SCAN_LINE_END) {
      // TODO(khanhdq): this shouldn't be here, it should be after mode 3
      if (!ppu_render(ppu)) {
        return false;
      }
      ppu_render_clear(ppu);
      ppu->scanline = 0;
      ppu->mode = 2;
    }
    break;
  }
  default: {
    FATAL("Invalid ppu mode %d", ppu->mode);
  }
  }

  return true;
}

void ppu_render_clear(Ppu *ppu) {
  for (int y = 0; y < WINDOW_HEIGHT; y++) {
    for (int x = 0; x < WINDOW_WIDTH; x++) {
      ppu->colors[(y * WINDOW_WIDTH) + x] = COLOR_WHITE;
    }
  }
  sdl_render_clear(&ppu->sdl);
}

bool ppu_render(Ppu *ppu) {
  // put all colors
  for (int y = 0; y < WINDOW_HEIGHT; y++) {
    for (int x = 0; x < WINDOW_WIDTH; x++) {
      ppu_put_color(ppu, x, y, &ppu->colors[(y * WINDOW_WIDTH) + x]);
    }
  }

  // show scan result on the screen
  if (sdl_render(&ppu->sdl) == SDL_APP_SUCCESS) {
    ppu_destroy(ppu);
    return false;
  }
  return true;
}

void ppu_put_color(const Ppu *ppu, int x, int y, Color *color) {
  sdl_put_color(&ppu->sdl, x, y, color);
}

/**
 * Where the current object at `*oam_ptr` can be cut by the `scanning_line`,
 * if it is, then load it to `*obj`
 */
bool oam_get_obj(const Mem *mem, const uint8_t *oam_ptr, Object *obj,
                 int scanning_line, int object_size) {
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

  // Byte 3: objibute flags
  obj->flags = *(oam_ptr + 3);

  // the actual offset y
  int off_y = scanning_line - lower_bound;
  int y_flipped = (obj->flags & OBJ_Y_FLIP_FLAG) != 0;

  // get the actual tile data and refine the off_y to that tile
  if (object_size == 8) {
    // the data is always in the first half, nothing to worry here
    get_tile_data_0x8000(mem, tile_index, &obj->tile);
    obj->off_y = y_flipped ? 7 - off_y : off_y;
  } else {
    // the data can be in the first half or the second half, depends on the
    // offset value and the flip condition

    if (off_y < 8 && !y_flipped) {
      // trivial: first half
      obj->off_y = off_y;
      get_tile_data_0x8000(mem, tile_index, &obj->tile);
    } else if (off_y < 8 && y_flipped) {
      // first half, but flipped to the second half
      obj->off_y = 7 - off_y;
      get_tile_data_0x8000(mem, tile_index + 1, &obj->tile);
    } else if (off_y >= 8 && !y_flipped) {
      // trivial: second half
      obj->off_y = off_y - 8;
      get_tile_data_0x8000(mem, tile_index + 1, &obj->tile);
    } else {
      // second half but flipped to the first half
      obj->off_y = 15 - off_y;
      get_tile_data_0x8000(mem, tile_index, &obj->tile);
    }
  }

  return true;
}

void ppu_oam_scan(Ppu *ppu) {
  ppu->oam_scan_result.n_objects = 0;

  uint8_t lcdc = mem_read_u8(ppu->mem, LCDC_ADDR);
  // the object is not enbaled, do not run oam scan here
  if ((lcdc & LCDC_OBJ_ENABLE_FLAG) == 0) {
    return;
  }

  int object_size = lcdc & LCDC_OBJ_SIZE_FLAG ? 16 : 8;

  Object *obj = ppu->oam_scan_result.object_buffer;
  const uint8_t *ptr = mem_ref_u8(ppu->mem, OAM_ADDR);
  for (int i = 0; i < OAM_MAX_OBJECTS; i++, ptr += 4) {
    if (oam_get_obj(ppu->mem, ptr, obj, ppu->scanline, object_size)) {
      obj++;
      ppu->oam_scan_result.n_objects++;
    }
  }

  if (ppu->oam_scan_result.n_objects > 0) {
    DEBUG("Scan line %d: Got %d objects", ppu->scanline,
          ppu->oam_scan_result.n_objects);
  }
}

[[nodiscard]] bool ppu_get_object_color(const Ppu *ppu, int x, Color *color);
[[nodiscard]] bool ppu_get_win_color(const Ppu *ppu, uint8_t lcdc, int x, int y,
                                     Color *color);
[[nodiscard]] bool ppu_get_bg_color(const Ppu *ppu, uint8_t lcdc, int x, int y,
                                    Color *color);

void ppu_draw_line_pixels(Ppu *ppu) {
  uint8_t lcdc = mem_read_u8(ppu->mem, LCDC_ADDR);
  int wx = mem_read_u8(ppu->mem, LCD_WIN_WX_ADDR);
  int wy = mem_read_u8(ppu->mem, LCD_WIN_WY_ADDR);
  wx -= 7;

  int scx = mem_read_u8(ppu->mem, LCD_BG_SCX_ADDR);
  int scy = mem_read_u8(ppu->mem, LCD_BG_SCY_ADDR);

  for (int x = 0; x < WINDOW_WIDTH; x++) {
    Color *color = &ppu->colors[(ppu->scanline * WINDOW_WIDTH) + x];

    if (ppu_get_object_color(ppu, x, color)) {
      continue;
    }

    if (x < wx) {
      if (!ppu_get_bg_color(ppu, lcdc, (x + scx) % 256,
                            (ppu->scanline + scy) % 256, color)) {
        *color = COLOR_WHITE;
      }
    } else {
      // has window here
      if (!ppu_get_win_color(ppu, lcdc, x - wx, ppu->scanline - wy, color)) {
        if (!ppu_get_bg_color(ppu, lcdc, (x + scx) % 256,
                              (ppu->scanline + scy) % 256, color)) {
          *color = COLOR_WHITE;
        }
      }
    }
  }
}

bool ppu_get_object_color(const Ppu *ppu, int x, Color *color) {
  // go through the collided objects in the scan line
  const Object *obj;
  for (obj = ppu->oam_scan_result.object_buffer;
       obj <
       ppu->oam_scan_result.object_buffer + ppu->oam_scan_result.n_objects;
       obj++) {
    // only only consider object lies in correct horizontal range
    if (obj->x - 8 <= x && x < obj->x) {
      int off_x = x - (obj->x - 8);
      if (obj->flags & OBJ_PRIORITY_FLIP_FLAG) {
        return false;
      }
      if (obj->flags & OBJ_X_FLIP_FLAG) {
        off_x = 7 - off_x;
      }
      if (obj->flags & OBJ_Y_FLIP_FLAG) {
        off_x = 7 - off_x;
      }
      return tile_color_at(&obj->tile, off_x, obj->off_y, color);
    }
  }
  return false;
}

[[nodiscard]]
bool get_bg_win_color(const Ppu *ppu, uint8_t lcdc, uint16_t tile_map_area_addr,
                      int x, int y, Color *color) {
  int tile_map_index_addr =
      tile_map_area_addr + ((y / 8) * TILE_MAP_WIDTH) + (x / 8);

  Tile tile;
  if ((lcdc & LCDC_BG_WIN_TILE_ADDR_FLAG) == 0) {
    get_tile_data_0x8800(
        ppu->mem, (int8_t)mem_read_u8(ppu->mem, tile_map_index_addr), &tile);
  } else {
    get_tile_data_0x8000(ppu->mem, mem_read_u8(ppu->mem, tile_map_index_addr),
                         &tile);
  }

  int off_x = x % 8;
  int off_y = y % 8;
  return tile_color_at(&tile, off_x, off_y, color);
}

bool ppu_get_win_color(const Ppu *ppu, uint8_t lcdc, int x, int y,
                       Color *color) {
  // BG and window is not enabled
  if ((lcdc & LCDC_BG_WIN_ENABLED_FLAG) == 0) {
    return false;
  }

  // window is not enabled
  if ((lcdc & LCDC_WIN_ENABLED_FLAG) == 0) {
    return false;
  }

  if (!ppu->win_y_condition) {
    return false;
  }

  int tile_map_area_addr =
      lcdc & LCDC_WIN_TILE_MAP_AREA_FLAG ? TILE_MAP1_ADDR : TILE_MAP0_ADDR;

  return get_bg_win_color(ppu, lcdc, tile_map_area_addr, x, y, color);
}

bool ppu_get_bg_color(const Ppu *ppu, uint8_t lcdc, int x, int y,
                      Color *color) {
  // BG and window is not enabled
  if ((lcdc & LCDC_BG_WIN_ENABLED_FLAG) == 0) {
    return false;
  }
  int tile_map_area_addr =
      lcdc & LCDC_BG_TILE_MAP_AREA_FLAG ? TILE_MAP1_ADDR : TILE_MAP0_ADDR;
  return get_bg_win_color(ppu, lcdc, tile_map_area_addr, x, y, color);
}
