#ifndef CONSTANT_H
#define CONSTANT_H

#define TIMA_ADDR 0xFF05
#define TMA_ADDR 0xFF06
#define TAC_ADDR 0xFF07

#define IE_ADDR 0xFFFF
#define IF_ADDR 0xFF0F

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
