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

void ppu_update_ly(Ppu *ppu);

// step the ppu
[[nodiscard]] bool ppu_step(Ppu *ppu, int t_cycle);

[[nodiscard]] bool ppu_render(Ppu *ppu);

void ppu_put_color(const Ppu *ppu, int x, int y, Color *color);

void ppu_oam(Ppu *ppu);
void ppu_draw(Ppu *ppu);

#endif // !PPU_H
