#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "SDL3/SDL_init.h"
#include "constant.h"
#include "macro.h"
#include "mem.h"
#include "ppu.h"
#include "ppu/color.h"
#include "ppu/oam.h"
#include "ppu/sdl.h"
#include "tile.h"

void ppu_init(Ppu *ppu, Mem *mem) {
  if (sdl_init(&ppu->sdl) == SDL_APP_FAILURE) {
    exit(1);
  };
  ppu->mem = mem;
  ppu->dots = 0;
  ppu->mode = PPU_MODE_START;
  ppu->scanline = 0;
  ppu->win_y = 0;
  ppu->win_y_condition = false;

  // init ly for scanline 0
  ppu_update_ly(ppu);
}

void ppu_destroy(Ppu *ppu) { sdl_quit(&ppu->sdl); }

PpuMode get_ppu_mode(int dots, int scanline) {
  if (scanline >= SCAN_LINE_VBLANK_START) {
    return PPU_MODE_VBLANK;
  }
  if (dots < MODE_OAM_END_DOT) {
    return PPU_MODE_OAM;
  }
  if (dots < MODE_DRAW_END_DOT) {
    return PPU_MODE_DRAW;
  }
  return PPU_MODE_HBLANK;
}

void ppu_enter_mode(Ppu *ppu, PpuMode mode) {
  uint8_t *stat = mem_ref_u8(ppu->mem, LCD_STAT_ADDR);
  *stat = (*stat & 0xFC) | (mode & 0x03);

  // check interrupt for ppu mode
  if (((*stat & LCD_STAT_MODE_0_SELECT_FLAG) != 0 && mode == PPU_MODE_HBLANK) ||
      ((*stat & LCD_STAT_MODE_1_SELECT_FLAG) != 0 && mode == PPU_MODE_VBLANK) ||
      ((*stat & LCD_STAT_MODE_2_SELECT_FLAG) != 0 && mode == PPU_MODE_OAM)) {
    request_interrupt(ppu->mem, I_LCD_FLAG);
  }

  switch (mode) {
  case PPU_MODE_OAM: {
    // ppu_oam(ppu);
    break;
  }
  case PPU_MODE_DRAW: {
    ppu_oam(ppu);
    ppu_draw(ppu);
    break;
  }
  case PPU_MODE_HBLANK: {
    break;
  }
  case PPU_MODE_VBLANK: {
    ppu->win_y_condition = false;
    ppu->win_y = 0;
    request_interrupt(ppu->mem, I_VBLANK_FLAG);
    break;
  }
  case PPU_MODE_START:
    FATAL("Invalid mode");
  }
}

void ppu_update_ly(Ppu *ppu) {
  uint8_t *stat = mem_ref_u8(ppu->mem, LCD_STAT_ADDR);
  uint8_t *ly = mem_ref_u8(ppu->mem, LCD_LY_ADDR);
  uint8_t lyc = mem_read_u8(ppu->mem, LCD_LYC_ADDR);

  *ly = ppu->scanline;

  if (*ly == lyc) {
    *stat |= LCD_STAT_LYC_EQ_LY_FLAG;
    if ((*stat & LCD_STAT_LYC_SELECT_FLAG) != 0 &&
        (*stat & LCD_STAT_LYC_EQ_LY_FLAG) != 0) {
      request_interrupt(ppu->mem, I_LCD_FLAG);
    }
  } else {
    *stat &= ~LCD_STAT_LYC_EQ_LY_FLAG;
  }

  int wy = mem_read_u8(ppu->mem, LCD_WIN_WY_ADDR);
  if (*ly == wy) {
    ppu->win_y_condition = true;
  }
}

bool ppu_step(Ppu *ppu, int t_cycle) {
  uint8_t lcdc = mem_read_u8(ppu->mem, LCDC_ADDR);
  if ((lcdc & LCDC_PPU_ENABLED_FLAG) == 0) {
    ppu->dots = 0;
    ppu->scanline = 0;
    ppu->win_y = 0;
    ppu->win_y_condition = false;

    uint8_t *ly = mem_ref_u8(ppu->mem, LCD_LY_ADDR);
    *ly = 0;
    return true;
  }

  ppu->dots += t_cycle;

  if (ppu->dots >= PPU_DOTS_PER_LINE) {
    ppu->dots -= PPU_DOTS_PER_LINE;
    ppu->scanline++;
    if (ppu->scanline >= SCAN_LINE_COUNT) {
      ppu->scanline = 0;
      if (!ppu_render(ppu)) {
        return false;
      }
    }
    ppu_update_ly(ppu);
  }

  PpuMode mode = get_ppu_mode(ppu->dots, ppu->scanline);
  if (mode != ppu->mode) {
    ppu_enter_mode(ppu, mode);
    ppu->mode = mode;
  }

  return true;
}

bool ppu_render(Ppu *ppu) {
  sdl_render_clear(&ppu->sdl);

  // put all colors
  for (int y = 0; y < WINDOW_HEIGHT; y++) {
    for (int x = 0; x < WINDOW_WIDTH; x++) {
      ppu_put_color(ppu, x, y, &ppu->colors[(y * WINDOW_WIDTH) + x]);
    }
  }

  // show scan result on the screen
  return sdl_render(&ppu->sdl) == SDL_APP_CONTINUE;
}

void ppu_put_color(const Ppu *ppu, int x, int y, Color *color) {
  sdl_put_color(&ppu->sdl, x, y, color);
}

void ppu_oam(Ppu *ppu) {
  ppu->oam_scan_result.n_objects = 0;

  uint8_t lcdc = mem_read_u8(ppu->mem, LCDC_ADDR);
  // the object is not enbaled, do not run oam scan here
  if ((lcdc & LCDC_OBJ_ENABLE_FLAG) == 0) {
    return;
  }

  int object_size = lcdc & LCDC_OBJ_SIZE_FLAG ? 16 : 8;

  Object *obj = ppu->oam_scan_result.object_buffer;
  const uint8_t *ptr = mem_ref_u8(ppu->mem, OAM_ADDR);
  for (int oam_index = 0;
       oam_index < OAM_MAX_OBJECTS &&
       ppu->oam_scan_result.n_objects < OAM_SCANNED_OBJ_LIMIT;
       oam_index++, ptr += 4) {
    if (oam_get_obj(ppu->mem, ptr, obj, oam_index, ppu->scanline,
                    object_size)) {
      obj++;
      ppu->oam_scan_result.n_objects++;
    }
  }

  oam_sort(ppu->oam_scan_result.object_buffer, ppu->oam_scan_result.n_objects);
}

typedef enum {
  USE_OBJ_COLOR,
  USE_BGP_COLOR,
  USE_WHITE_COLOR,
} UseColor;

[[nodiscard]]
UseColor ppu_get_object_color_index(Ppu *ppu, int x, ColorIndex *ci,
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

      // if the color index is 0, then this is transparent color, do not use
      // it!
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
UseColor ppu_get_bg_win_color_index(Ppu *ppu, uint8_t lcdc, int x, int wx,
                                    int scx, int scy, ColorIndex *ci,
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
    y = ppu->win_y;
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

void ppu_draw(Ppu *ppu) {
  Palette obj0_palette;
  Palette obj1_palette;
  Palette bgp_palette;

  color_get_palette_bgp(ppu->mem, &bgp_palette);
  color_get_palette_obj0(ppu->mem, &obj0_palette);
  color_get_palette_obj1(ppu->mem, &obj1_palette);

  uint8_t lcdc = mem_read_u8(ppu->mem, LCDC_ADDR);
  int wx = mem_read_u8(ppu->mem, LCD_WIN_WX_ADDR);
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

    // if we don't use object color then we need to fetch color from
    // background There are 2 cases:
    // - we only use background color: fetch it
    // - we prefer color index 1-3 from background color, **try** to fetch it!
    if (use_color != USE_OBJ_COLOR) {
      use_color = ppu_get_bg_win_color_index(ppu, lcdc, x, wx, scx, scy,
                                             &bgp_ci, &bgp_palette);
    } else if (prefer_bg) {
      UseColor use_bg_color = ppu_get_bg_win_color_index(
          ppu, lcdc, x, wx, scx, scy, &bgp_ci, &bgp_palette);
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

  // increment window counter in this scanline
  // BG and window is not enabled
  if ((lcdc & LCDC_BG_WIN_ENABLED_FLAG) != 0 &&
      (lcdc & LCDC_WIN_ENABLED_FLAG) != 0 && (int)ppu->win_y_condition &&
      0 <= wx && wx < WINDOW_WIDTH) {
    ppu->win_y++;
  }
}
