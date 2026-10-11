#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>

#include "SDL3/SDL_init.h"
#include "constant.h"
#include "macro.h"
#include "mem.h"
#include "ppu.h"
#include "ppu/color.h"
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

typedef enum {
  USE_OBJ_COLOR,
  USE_BGP_COLOR,
  USE_WHITE_COLOR,
} UseColor;

[[nodiscard]]
UseColor ppu_get_object_color_index(const Ppu *ppu, int x, ColorIndex *ci,
                                    Palette *obj0_palette,
                                    Palette *obj1_palette, bool *prefer_bg) {
  // go through the collided objects in the scan line
  const Object *obj;
  for (obj = ppu->oam_scan_result.object_buffer;
       obj <
       ppu->oam_scan_result.object_buffer + ppu->oam_scan_result.n_objects;
       obj++) {
    // only only consider object lies in correct horizontal range
    if (obj->x - 8 <= x && x < obj->x) {
      int off_x = x - (obj->x - 8);
      if (obj->flags & OBJ_X_FLIP_FLAG) {
        off_x = 7 - off_x;
      }

      if ((obj->flags & OBJ_DMG_PALETTE_FLAG) == 0) {
        *ci = tile_color_index(&obj->tile, off_x, obj->off_y, obj0_palette);
      } else {
        *ci = tile_color_index(&obj->tile, off_x, obj->off_y, obj1_palette);
      }

      // if the color index is 0, then this is transparent color, do not use it!
      if (ci->index == 0) {
        continue;
      }

      *prefer_bg = (obj->flags & OBJ_PRIORITY_FLIP_FLAG) != 0;
      return USE_OBJ_COLOR;
    }
  }

  return USE_BGP_COLOR;
}

[[nodiscard]]
UseColor ppu_get_bg_win_color_index(const Ppu *ppu, uint8_t lcdc, int x, int wx,
                                    int wy, int scx, int scy, ColorIndex *ci,
                                    Palette *bgp_palette) {
  // BG and window is not enabled
  if ((lcdc & LCDC_BG_WIN_ENABLED_FLAG) == 0) {
    return USE_WHITE_COLOR;
  }

  int tile_map_area_addr;
  int y;

  if ((lcdc & LCDC_WIN_ENABLED_FLAG) != 0 && (int)ppu->win_y_condition &&
      x >= wx) {
    // window is enabled, use the color from window instead
    tile_map_area_addr =
        lcdc & LCDC_WIN_TILE_MAP_AREA_FLAG ? TILE_MAP1_ADDR : TILE_MAP0_ADDR;
    x -= wx;
    y = ppu->scanline - wy;
  } else {
    // background
    tile_map_area_addr =
        lcdc & LCDC_BG_TILE_MAP_AREA_FLAG ? TILE_MAP1_ADDR : TILE_MAP0_ADDR;
    x = (x + scx) % 256;
    y = (ppu->scanline + scy) % 256;
  }

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
  *ci = tile_color_index(&tile, off_x, off_y, bgp_palette);
  return USE_BGP_COLOR;
}

void ppu_draw_line_pixels(Ppu *ppu) {
  Palette obj0_palette;
  Palette obj1_palette;
  Palette bgp_palette;

  color_get_palette_bgp(ppu->mem, &bgp_palette);
  color_get_palette_obj0(ppu->mem, &obj0_palette);
  color_get_palette_obj1(ppu->mem, &obj1_palette);

  uint8_t lcdc = mem_read_u8(ppu->mem, LCDC_ADDR);
  int wx = mem_read_u8(ppu->mem, LCD_WIN_WX_ADDR);
  int wy = mem_read_u8(ppu->mem, LCD_WIN_WY_ADDR);
  wx -= 7;

  int scx = mem_read_u8(ppu->mem, LCD_BG_SCX_ADDR);
  int scy = mem_read_u8(ppu->mem, LCD_BG_SCY_ADDR);

  ColorIndex obj_ci;
  ColorIndex bgp_ci;
  bool prefer_bg;

  for (int x = 0; x < WINDOW_WIDTH; x++) {
    Color *color = &ppu->colors[(ppu->scanline * WINDOW_WIDTH) + x];
    UseColor use_color = ppu_get_object_color_index(
        ppu, x, &obj_ci, &obj0_palette, &obj1_palette, &prefer_bg);

    // if we don't use object color then we need to fetch color from background
    // There are 2 cases:
    // - we only use background color: fetch it
    // - we prefer color index 1-3 from background color, **try** to fetch it!
    if (use_color != USE_OBJ_COLOR) {
      use_color = ppu_get_bg_win_color_index(ppu, lcdc, x, wx, wy, scx, scy,
                                             &bgp_ci, &bgp_palette);
    } else if (prefer_bg) {
      UseColor use_bg_color = ppu_get_bg_win_color_index(
          ppu, lcdc, x, wx, wy, scx, scy, &bgp_ci, &bgp_palette);
      // we only use color from background color if the index is 1-3
      // https://gbdev.io/pandocs/OAM.html#byte-3--attributesflags
      if (use_bg_color == USE_BGP_COLOR && bgp_ci.index != 0) {
        use_color = USE_BGP_COLOR;
      }
    }

    switch (use_color) {
    case USE_OBJ_COLOR: {
      color_from_index(&obj_ci, color);
      break;
    }
    case USE_BGP_COLOR: {
      color_from_index(&bgp_ci, color);
      break;
    }
    case USE_WHITE_COLOR:
      *color = COLOR_WHITE;
      break;
    }
  }
}
