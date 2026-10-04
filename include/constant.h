#ifndef CONSTANT_H
#define CONSTANT_H

#include <stdint.h>

typedef struct {
  uint8_t r, g, b;
} Color;

static const Color COLOR_WHILE = {255, 255, 255};

static const Color color_mapping[4] = {
    {255, 255, 255},
    {170, 170, 170},
    {85, 85, 85},
    {0, 0, 0},
};

#define WINDOW_HEIGHT 144
#define WINDOW_WIDTH 160

#define TILE_MAP_WIDTH 32
#define TILE_MAP_HEIGHT 32
#define TILE_MAP0_ADDR 0x9800
#define TILE_MAP1_ADDR 0x9C00

#define TILE_BLOCK0_ADDR 0x8000
#define TILE_BLOCK1_ADDR 0x9000
#define TILE_SIZE 16

#define LCDC_ADDR 0xFF40
#define LCDC_BG_WIN_ENABLED_FLAG 1
#define LCDC_OBJ_ENABLE_FLAG 2
#define LCDC_OBJ_SIZE_FLAG 4
#define LCDC_BG_TILE_MAP_AREA_FLAG 8
#define LCDC_BG_WIN_TILE_ADDR_FLAG 16
#define LCDC_WIN_ENABLED_FLAG 32
#define LCDC_WIN_TILE_MAP_AREA_FLAG 64

#define OAM_ADDR 0xFE00
#define OAM_MAX_OBJECTS 40
#define OBJ_DMG_PALETTE_FLAG 16
#define OBJ_X_FLIP_FLAG 32
#define OBJ_Y_FLIP_FLAG 64
#define OBJ_PRIORITY_FLIP_FLAG 128

#define TIMA_ADDR 0xFF05
#define TMA_ADDR 0xFF06
#define TAC_ADDR 0xFF07

#define IE_ADDR 0xFFFF
#define IF_ADDR 0xFF0F

#define TAC_ENABLED_FLAG 4

#define I_VBLANK_ADDR 0x40
#define I_STAT_ADDR 0x48
#define I_TIMER_ADDR 0x50
#define I_SERIAL_ADDR 0x58
#define I_JOYPAD_ADDR 0x60

#define I_VBLANK_FLAG 1
#define I_LCD_FLAG 2
#define I_TIMER_FLAG 4
#define I_SERIAL_FLAG 8
#define I_JOYPAD_FLAG 16

#endif // !CONSTANT_H
