#include "emu.h"

int main(int /*argc*/, char **argv) {
  Emu emu;
  const char *filename = argv[1];
  emu_init(&emu, filename);
  emu_loop(&emu);
  emu_destroy(&emu);
  return 0;
}
