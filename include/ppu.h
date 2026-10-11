#ifndef PPU_H
#define PPU_H

#include <stdint.h>

#include "constant.h"
#include "mem.h"
#include "ppu/color.h"
#include "ppu/oam.h"
#include "ppu/sdl.h"

typedef enum {
  PPU_MODE_START,  // undefined
  PPU_MODE_OAM,    // mode 2
  PPU_MODE_DRAW,   // mode 3
  PPU_MODE_HBLANK, // mode 0
  PPU_MODE_VBLANK, // mode 1
} PpuMode;

typedef struct {
  Mem *mem;
  SdlWrapper sdl;

  int dots;
  PpuMode mode;
  int scanline;                     // 0 - 153
  PpuOAMScanResult oam_scan_result; // temporary oam scan result
  bool win_y_condition;
  // colors to render
  Color colors[WINDOW_WIDTH * WINDOW_HEIGHT];
} Ppu;

void ppu_init(Ppu *ppu, Mem *mem);
void ppu_destroy(Ppu *ppu);

// step the ppu
[[nodiscard]] bool ppu_step(Ppu *ppu, int t_cycle);

[[nodiscard]] bool ppu_render(Ppu *ppu);

void ppu_put_color(const Ppu *ppu, int x, int y, Color *color);

/**
 * Run the scanline. This includes:
 * - Mode 2: OAM scan
 * - Mode 3: Drawing pixels
 * - Mode 0: Horizontal blank
 */
void ppu_scan_line(Ppu *ppu, int line, uint8_t *ly, uint8_t lyc, uint8_t *stat,
                   uint8_t *if_);
void ppu_oam_scan(Ppu *ppu);
void ppu_draw_line_pixels(Ppu *ppu);

#endif // !PPU_H
