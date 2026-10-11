#ifndef PPU_H
#define PPU_H

#include <stdint.h>

#include "constant.h"
#include "mem.h"
#include "ppu/color.h"
#include "ppu/oam.h"
#include "ppu/sdl.h"

typedef struct {
  bool running;
  Mem *mem;
  SdlWrapper sdl;

  int mode;                         // 0 1 2 3
  int scanline;                     // 0 - 153
  PpuOAMScanResult oam_scan_result; // temporary oam scan result
  bool win_y_condition;
  // colors to render
  Color colors[WINDOW_WIDTH * WINDOW_HEIGHT];
} Ppu;

void ppu_init(Ppu *ppu, Mem *mem);
void ppu_destroy(Ppu *ppu);

// step the ppu
[[nodiscard]] bool ppu_step(Ppu *ppu);

void ppu_render_clear(Ppu *ppu);
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
