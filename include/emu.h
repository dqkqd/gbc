#ifndef EMU_H
#define EMU_H

#include "cpu.h"
#include "mem.h"

typedef struct {
  Mem mem;
  Cpu cpu;
} Emu;

int emu_init(Emu *emu, const char *rom);
void emu_destroy(Emu *emu);
void emu_loop(Emu *emu);

#endif // !EMU_H
