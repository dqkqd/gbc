#ifndef MACRO_H
#define MACRO_H

#include <stdio.h>
#include <stdlib.h> // IWYU pragma: keep

#define DEBUG(fmt, ...)                                                        \
  do {                                                                         \
    fprintf(stderr, fmt "\n" __VA_OPT__(, ) __VA_ARGS__);                      \
  } while (0)

#define FATAL(fmt, ...)                                                        \
  do {                                                                         \
    fprintf(stderr, fmt "\n" __VA_OPT__(, ) __VA_ARGS__);                      \
    fflush(stderr);                                                            \
    exit(1);                                                                   \
  } while (0)

#ifdef GB_TEST

static FILE *gb_doctor_log = nullptr;
static inline void gb_doctor_log_init() {
  const char *logfile = "logs/gameboy_doctor.log";
  gb_doctor_log = fopen(logfile, "w");
  if (!gb_doctor_log) {
    perror(logfile);
    _Exit(1);
  }
}

static inline void gb_doctor_log_close() { fclose(gb_doctor_log); }

static inline void gb_doctor_log_close_signal(int sig) {
  gb_doctor_log_close();
  _Exit(128 + sig);
}

#define GAMEBOY_DOCTOR(emu)                                                    \
  do {                                                                         \
    fprintf(gb_doctor_log,                                                     \
            "A:%02X F:%02X B:%02X C:%02X D:%02X E:%02X H:%02X L:%02X SP:%04X " \
            "PC:%04X PCMEM:%02X,%02X,%02X,%02X\n",                             \
            (emu)->cpu.reg.a, reg_f(&(emu)->cpu.reg), (emu)->cpu.reg.b,        \
            (emu)->cpu.reg.c, (emu)->cpu.reg.d, (emu)->cpu.reg.e,              \
            (emu)->cpu.reg.h, (emu)->cpu.reg.l, (emu)->cpu.reg.sp,             \
            (emu)->cpu.reg.pc, mem_read_u8(&(emu)->mem, (emu)->cpu.reg.pc),    \
            mem_read_u8(&(emu)->mem, (emu)->cpu.reg.pc + 1),                   \
            mem_read_u8(&(emu)->mem, (emu)->cpu.reg.pc + 2),                   \
            mem_read_u8(&(emu)->mem, (emu)->cpu.reg.pc + 3));                  \
  } while (0)
#else
#define GAMEBOY_DOCTOR(emu)                                                    \
  do {                                                                         \
  } while (0)
#endif // GB_TEST

#endif // !MACRO_H
