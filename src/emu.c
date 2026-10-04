#include "emu.h"
#include "cpu.h"
#include "macro.h"
#include "mem.h"
#include "opcode.h"
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int emu_init(Emu *emu, const char *rom) {
  FILE *f = fopen(rom, "rb");
  if (!f) {
    perror(rom);
    return 1;
  }

  cpu_init(&emu->cpu);
  mem_init(&emu->mem, f);
  fclose(f);

#if GB_TEST
  gb_doctor_log_init();
  atexit(gb_doctor_log_close);
  signal(SIGINT, gb_doctor_log_close_signal); // NOLINT
#endif

  return 0;
}

void emu_destroy(Emu *emu) { mem_destroy(&emu->mem); }

static uint8_t read_u8(Emu *emu) {
  uint8_t res = mem_read_u8(&emu->mem, emu->cpu.reg.pc);
  if ((int)emu->cpu.halt_bug) {
    emu->cpu.halt_bug = false;
  } else {
    emu->cpu.reg.pc++;
  }
  return res;
}

static int8_t read_i8(Emu *emu) { return (int8_t)read_u8(emu); }

static uint16_t read_u16(Emu *emu) {
  uint16_t res = mem_read_u16(&emu->mem, emu->cpu.reg.pc);
  emu->cpu.reg.pc += 2;
  return res;
}

void decode(Emu *emu, Opcode *opcode) { // NOLINT
  uint8_t kind = read_u8(emu);
  switch (kind) {
  case 0x00: {
    opcode->kind = OPCODE_KIND_0x00_NOP;
    break;
  }
  case 0x01: {
    opcode->kind = OPCODE_KIND_0x01_LD_BC_n16;
    opcode->n16 = read_u16(emu);
    break;
  }

  case 0x06: {
    opcode->kind = OPCODE_KIND_0x06_LD_B_n8;
    opcode->n8 = read_u8(emu);
    break;
  }
  case 0x0e: {
    opcode->kind = OPCODE_KIND_0x0e_LD_C_n8;
    opcode->n8 = read_u8(emu);
    break;
  }
  case 0x16: {
    opcode->kind = OPCODE_KIND_0x16_LD_D_n8;
    opcode->n8 = read_u8(emu);
    break;
  }
  case 0x1e: {
    opcode->kind = OPCODE_KIND_0x1e_LD_E_n8;
    opcode->n8 = read_u8(emu);
    break;
  }
  case 0x26: {
    opcode->kind = OPCODE_KIND_0x26_LD_H_n8;
    opcode->n8 = read_u8(emu);
    break;
  }
  case 0x2e: {
    opcode->kind = OPCODE_KIND_0x2e_LD_L_n8;
    opcode->n8 = read_u8(emu);
    break;
  }
  case 0x36: {
    opcode->kind = OPCODE_KIND_0x36_LD_pHL_n8;
    opcode->n8 = read_u8(emu);
    break;
  }
  case 0x3e: {
    opcode->kind = OPCODE_KIND_0x3e_LD_A_n8;
    opcode->n8 = read_u8(emu);
    break;
  }

  case 0x11: {
    opcode->kind = OPCODE_KIND_0x11_LD_DE_n16;
    opcode->n16 = read_u16(emu);
    break;
  }
  case 0x21: {
    opcode->kind = OPCODE_KIND_0x21_LD_HL_n16;
    opcode->n16 = read_u16(emu);
    break;
  }
  case 0x31: {
    opcode->kind = OPCODE_KIND_0x31_LD_SP_n16;
    opcode->n16 = read_u16(emu);
    break;
  }
  case 0x40: {
    opcode->kind = OPCODE_KIND_0x40_LD_B_B;
    break;
  }
  case 0x41: {
    opcode->kind = OPCODE_KIND_0x41_LD_B_C;
    break;
  }
  case 0x42: {
    opcode->kind = OPCODE_KIND_0x42_LD_B_D;
    break;
  }
  case 0x43: {
    opcode->kind = OPCODE_KIND_0x43_LD_B_E;
    break;
  }
  case 0x44: {
    opcode->kind = OPCODE_KIND_0x44_LD_B_H;
    break;
  }
  case 0x45: {
    opcode->kind = OPCODE_KIND_0x45_LD_B_L;
    break;
  }
  case 0x46: {
    opcode->kind = OPCODE_KIND_0x46_LD_B_pHL;
    break;
  }
  case 0x47: {
    opcode->kind = OPCODE_KIND_0x47_LD_B_A;
    break;
  }
  case 0x48: {
    opcode->kind = OPCODE_KIND_0x48_LD_C_B;
    break;
  }
  case 0x49: {
    opcode->kind = OPCODE_KIND_0x49_LD_C_C;
    break;
  }
  case 0x4a: {
    opcode->kind = OPCODE_KIND_0x4a_LD_C_D;
    break;
  }
  case 0x4b: {
    opcode->kind = OPCODE_KIND_0x4b_LD_C_E;
    break;
  }
  case 0x4c: {
    opcode->kind = OPCODE_KIND_0x4c_LD_C_H;
    break;
  }
  case 0x4d: {
    opcode->kind = OPCODE_KIND_0x4d_LD_C_L;
    break;
  }
  case 0x4e: {
    opcode->kind = OPCODE_KIND_0x4e_LD_C_pHL;
    break;
  }
  case 0x4f: {
    opcode->kind = OPCODE_KIND_0x4f_LD_C_A;
    break;
  }
  case 0x50: {
    opcode->kind = OPCODE_KIND_0x50_LD_D_B;
    break;
  }
  case 0x51: {
    opcode->kind = OPCODE_KIND_0x51_LD_D_C;
    break;
  }
  case 0x52: {
    opcode->kind = OPCODE_KIND_0x52_LD_D_D;
    break;
  }
  case 0x53: {
    opcode->kind = OPCODE_KIND_0x53_LD_D_E;
    break;
  }
  case 0x54: {
    opcode->kind = OPCODE_KIND_0x54_LD_D_H;
    break;
  }
  case 0x55: {
    opcode->kind = OPCODE_KIND_0x55_LD_D_L;
    break;
  }
  case 0x56: {
    opcode->kind = OPCODE_KIND_0x56_LD_D_pHL;
    break;
  }
  case 0x57: {
    opcode->kind = OPCODE_KIND_0x57_LD_D_A;
    break;
  }
  case 0x58: {
    opcode->kind = OPCODE_KIND_0x58_LD_E_B;
    break;
  }
  case 0x59: {
    opcode->kind = OPCODE_KIND_0x59_LD_E_C;
    break;
  }
  case 0x5a: {
    opcode->kind = OPCODE_KIND_0x5a_LD_E_D;
    break;
  }
  case 0x5b: {
    opcode->kind = OPCODE_KIND_0x5b_LD_E_E;
    break;
  }
  case 0x5c: {
    opcode->kind = OPCODE_KIND_0x5c_LD_E_H;
    break;
  }
  case 0x5d: {
    opcode->kind = OPCODE_KIND_0x5d_LD_E_L;
    break;
  }
  case 0x5e: {
    opcode->kind = OPCODE_KIND_0x5e_LD_E_pHL;
    break;
  }
  case 0x5f: {
    opcode->kind = OPCODE_KIND_0x5f_LD_E_A;
    break;
  }
  case 0x60: {
    opcode->kind = OPCODE_KIND_0x60_LD_H_B;
    break;
  }
  case 0x61: {
    opcode->kind = OPCODE_KIND_0x61_LD_H_C;
    break;
  }
  case 0x62: {
    opcode->kind = OPCODE_KIND_0x62_LD_H_D;
    break;
  }
  case 0x63: {
    opcode->kind = OPCODE_KIND_0x63_LD_H_E;
    break;
  }
  case 0x64: {
    opcode->kind = OPCODE_KIND_0x64_LD_H_H;
    break;
  }
  case 0x65: {
    opcode->kind = OPCODE_KIND_0x65_LD_H_L;
    break;
  }
  case 0x66: {
    opcode->kind = OPCODE_KIND_0x66_LD_H_pHL;
    break;
  }
  case 0x67: {
    opcode->kind = OPCODE_KIND_0x67_LD_H_A;
    break;
  }
  case 0x68: {
    opcode->kind = OPCODE_KIND_0x68_LD_L_B;
    break;
  }
  case 0x69: {
    opcode->kind = OPCODE_KIND_0x69_LD_L_C;
    break;
  }
  case 0x6a: {
    opcode->kind = OPCODE_KIND_0x6a_LD_L_D;
    break;
  }
  case 0x6b: {
    opcode->kind = OPCODE_KIND_0x6b_LD_L_E;
    break;
  }
  case 0x6c: {
    opcode->kind = OPCODE_KIND_0x6c_LD_L_H;
    break;
  }
  case 0x6d: {
    opcode->kind = OPCODE_KIND_0x6d_LD_L_L;
    break;
  }
  case 0x6e: {
    opcode->kind = OPCODE_KIND_0x6e_LD_L_pHL;
    break;
  }
  case 0x6f: {
    opcode->kind = OPCODE_KIND_0x6f_LD_L_A;
    break;
  }
  case 0x70: {
    opcode->kind = OPCODE_KIND_0x70_LD_pHL_B;
    break;
  }
  case 0x71: {
    opcode->kind = OPCODE_KIND_0x71_LD_pHL_C;
    break;
  }
  case 0x72: {
    opcode->kind = OPCODE_KIND_0x72_LD_pHL_D;
    break;
  }
  case 0x73: {
    opcode->kind = OPCODE_KIND_0x73_LD_pHL_E;
    break;
  }
  case 0x74: {
    opcode->kind = OPCODE_KIND_0x74_LD_pHL_H;
    break;
  }
  case 0x75: {
    opcode->kind = OPCODE_KIND_0x75_LD_pHL_L;
    break;
  }
  case 0x77: {
    opcode->kind = OPCODE_KIND_0x77_LD_pHL_A;
    break;
  }
  case 0x78: {
    opcode->kind = OPCODE_KIND_0x78_LD_A_B;
    break;
  }
  case 0x79: {
    opcode->kind = OPCODE_KIND_0x79_LD_A_C;
    break;
  }
  case 0x7a: {
    opcode->kind = OPCODE_KIND_0x7a_LD_A_D;
    break;
  }
  case 0x7b: {
    opcode->kind = OPCODE_KIND_0x7b_LD_A_E;
    break;
  }
  case 0x7c: {
    opcode->kind = OPCODE_KIND_0x7c_LD_A_H;
    break;
  }
  case 0x7d: {
    opcode->kind = OPCODE_KIND_0x7d_LD_A_L;
    break;
  }
  case 0x7e: {
    opcode->kind = OPCODE_KIND_0x7e_LD_A_pHL;
    break;
  }
  case 0x7f: {
    opcode->kind = OPCODE_KIND_0x7f_LD_A_A;
    break;
  }
  case 0xc3: {
    opcode->kind = OPCODE_KIND_0xc3_JP_a16;
    opcode->a16 = read_u16(emu);
    break;
  }
  case 0x0a: {
    opcode->kind = OPCODE_KIND_0x0a_LD_A_pBC;
    break;
  }
  case 0x1a: {
    opcode->kind = OPCODE_KIND_0x1a_LD_A_pDE;
    break;
  }
  case 0x2a: {
    opcode->kind = OPCODE_KIND_0x2a_LD_A_pHLi;
    break;
  }
  case 0x3a: {
    opcode->kind = OPCODE_KIND_0x3a_LD_A_pHLd;
    break;
  }
  case 0x02: {
    opcode->kind = OPCODE_KIND_0x02_LD_pBC_A;
    break;
  }
  case 0x12: {
    opcode->kind = OPCODE_KIND_0x12_LD_pDE_A;
    break;
  }
  case 0x22: {
    opcode->kind = OPCODE_KIND_0x22_LD_pHLi_A;
    break;
  }
  case 0x32: {
    opcode->kind = OPCODE_KIND_0x32_LD_pHLd_A;
    break;
  }
  case 0x04: {
    opcode->kind = OPCODE_KIND_0x04_INC_B;
    break;
  }
  case 0x0c: {
    opcode->kind = OPCODE_KIND_0x0c_INC_C;
    break;
  }
  case 0x14: {
    opcode->kind = OPCODE_KIND_0x14_INC_D;
    break;
  }
  case 0x1c: {
    opcode->kind = OPCODE_KIND_0x1c_INC_E;
    break;
  }
  case 0x24: {
    opcode->kind = OPCODE_KIND_0x24_INC_H;
    break;
  }
  case 0x2c: {
    opcode->kind = OPCODE_KIND_0x2c_INC_L;
    break;
  }
  case 0x34: {
    opcode->kind = OPCODE_KIND_0x34_INC_pHL;
    break;
  }
  case 0x3c: {
    opcode->kind = OPCODE_KIND_0x3c_INC_A;
    break;
  }
  case 0x05: {
    opcode->kind = OPCODE_KIND_0x05_DEC_B;
    break;
  }
  case 0x0d: {
    opcode->kind = OPCODE_KIND_0x0d_DEC_C;
    break;
  }
  case 0x15: {
    opcode->kind = OPCODE_KIND_0x15_DEC_D;
    break;
  }
  case 0x1d: {
    opcode->kind = OPCODE_KIND_0x1d_DEC_E;
    break;
  }
  case 0x25: {
    opcode->kind = OPCODE_KIND_0x25_DEC_H;
    break;
  }
  case 0x2d: {
    opcode->kind = OPCODE_KIND_0x2d_DEC_L;
    break;
  }
  case 0x35: {
    opcode->kind = OPCODE_KIND_0x35_DEC_pHL;
    break;
  }
  case 0x3d: {
    opcode->kind = OPCODE_KIND_0x3d_DEC_A;
    break;
  }
  case 0x18: {
    opcode->kind = OPCODE_KIND_0x18_JR_e8;
    opcode->e8 = read_i8(emu);
    break;
  }
  case 0x28: {
    opcode->kind = OPCODE_KIND_0x28_JR_Z_e8;
    opcode->e8 = read_i8(emu);
    break;
  }
  case 0x38: {
    opcode->kind = OPCODE_KIND_0x38_JR_C_e8;
    opcode->e8 = read_i8(emu);
    break;
  }
  case 0x20: {
    opcode->kind = OPCODE_KIND_0x20_JR_NZ_e8;
    opcode->e8 = read_i8(emu);
    break;
  }
  case 0x30: {
    opcode->kind = OPCODE_KIND_0x30_JR_NC_e8;
    opcode->e8 = read_i8(emu);
    break;
  }
  case 0xf3: {
    opcode->kind = OPCODE_KIND_0xf3_DI;
    break;
  }
  case 0xfb: {
    opcode->kind = OPCODE_KIND_0xfb_EI;
    break;
  }
  case 0xea: {
    opcode->kind = OPCODE_KIND_0xea_LD_pa16_A;
    opcode->a16 = read_u16(emu);
    break;
  }
  case 0xfa: {
    opcode->kind = OPCODE_KIND_0xfa_LD_A_pa16;
    opcode->a16 = read_u16(emu);
    break;
  }
  case 0xe0: {
    opcode->kind = OPCODE_KIND_0xe0_LDH_pa8_A;
    opcode->a8 = read_u8(emu);
    break;
  }
  case 0xf0: {
    opcode->kind = OPCODE_KIND_0xf0_LDH_A_pa8;
    opcode->a8 = read_u8(emu);
    break;
  }
  case 0xe2: {
    opcode->kind = OPCODE_KIND_0xe2_LDH_pC_A;
    break;
  }
  case 0xf2: {
    opcode->kind = OPCODE_KIND_0xf2_LDH_A_pC;
    break;
  }
  case 0xcd: {
    opcode->kind = OPCODE_KIND_0xcd_CALL_a16;
    opcode->a16 = read_u16(emu);
    break;
  }
  case 0xcc: {
    opcode->kind = OPCODE_KIND_0xcc_CALL_Z_a16;
    opcode->a16 = read_u16(emu);
    break;
  }
  case 0xdc: {
    opcode->kind = OPCODE_KIND_0xdc_CALL_C_a16;
    opcode->a16 = read_u16(emu);
    break;
  }
  case 0xc4: {
    opcode->kind = OPCODE_KIND_0xc4_CALL_NZ_a16;
    opcode->a16 = read_u16(emu);
    break;
  }
  case 0xd4: {
    opcode->kind = OPCODE_KIND_0xd4_CALL_NC_a16;
    opcode->a16 = read_u16(emu);
    break;
  }
  case 0xc9: {
    opcode->kind = OPCODE_KIND_0xc9_RET;
    break;
  }
  case 0xc8: {
    opcode->kind = OPCODE_KIND_0xc8_RET_Z;
    break;
  }
  case 0xd8: {
    opcode->kind = OPCODE_KIND_0xd8_RET_C;
    break;
  }
  case 0xc0: {
    opcode->kind = OPCODE_KIND_0xc0_RET_NZ;
    break;
  }
  case 0xd0: {
    opcode->kind = OPCODE_KIND_0xd0_RET_NC;
    break;
  }
  case 0xc5: {
    opcode->kind = OPCODE_KIND_0xc5_PUSH_BC;
    break;
  }
  case 0xd5: {
    opcode->kind = OPCODE_KIND_0xd5_PUSH_DE;
    break;
  }
  case 0xe5: {
    opcode->kind = OPCODE_KIND_0xe5_PUSH_HL;
    break;
  }
  case 0xf5: {
    opcode->kind = OPCODE_KIND_0xf5_PUSH_AF;
    break;
  }
  case 0xc1: {
    opcode->kind = OPCODE_KIND_0xc1_POP_BC;
    break;
  }
  case 0xd1: {
    opcode->kind = OPCODE_KIND_0xd1_POP_DE;
    break;
  }
  case 0xe1: {
    opcode->kind = OPCODE_KIND_0xe1_POP_HL;
    break;
  }
  case 0xf1: {
    opcode->kind = OPCODE_KIND_0xf1_POP_AF;
    break;
  }
  case 0x03: {
    opcode->kind = OPCODE_KIND_0x03_INC_BC;
    break;
  }
  case 0x13: {
    opcode->kind = OPCODE_KIND_0x13_INC_DE;
    break;
  }
  case 0x23: {
    opcode->kind = OPCODE_KIND_0x23_INC_HL;
    break;
  }
  case 0x33: {
    opcode->kind = OPCODE_KIND_0x33_INC_SP;
    break;
  }
  case 0x0b: {
    opcode->kind = OPCODE_KIND_0x0b_DEC_BC;
    break;
  }
  case 0x1b: {
    opcode->kind = OPCODE_KIND_0x1b_DEC_DE;
    break;
  }
  case 0x2b: {
    opcode->kind = OPCODE_KIND_0x2b_DEC_HL;
    break;
  }
  case 0x3b: {
    opcode->kind = OPCODE_KIND_0x3b_DEC_SP;
    break;
  }
  case 0x80: {
    opcode->kind = OPCODE_KIND_0x80_ADD_A_B;
    break;
  }
  case 0x81: {
    opcode->kind = OPCODE_KIND_0x81_ADD_A_C;
    break;
  }
  case 0x82: {
    opcode->kind = OPCODE_KIND_0x82_ADD_A_D;
    break;
  }
  case 0x83: {
    opcode->kind = OPCODE_KIND_0x83_ADD_A_E;
    break;
  }
  case 0x84: {
    opcode->kind = OPCODE_KIND_0x84_ADD_A_H;
    break;
  }
  case 0x85: {
    opcode->kind = OPCODE_KIND_0x85_ADD_A_L;
    break;
  }
  case 0x86: {
    opcode->kind = OPCODE_KIND_0x86_ADD_A_pHL;
    break;
  }
  case 0x87: {
    opcode->kind = OPCODE_KIND_0x87_ADD_A_A;
    break;
  }
  case 0x88: {
    opcode->kind = OPCODE_KIND_0x88_ADC_A_B;
    break;
  }
  case 0x89: {
    opcode->kind = OPCODE_KIND_0x89_ADC_A_C;
    break;
  }
  case 0x8a: {
    opcode->kind = OPCODE_KIND_0x8a_ADC_A_D;
    break;
  }
  case 0x8b: {
    opcode->kind = OPCODE_KIND_0x8b_ADC_A_E;
    break;
  }
  case 0x8c: {
    opcode->kind = OPCODE_KIND_0x8c_ADC_A_H;
    break;
  }
  case 0x8d: {
    opcode->kind = OPCODE_KIND_0x8d_ADC_A_L;
    break;
  }
  case 0x8e: {
    opcode->kind = OPCODE_KIND_0x8e_ADC_A_pHL;
    break;
  }
  case 0x8f: {
    opcode->kind = OPCODE_KIND_0x8f_ADC_A_A;
    break;
  }
  case 0x90: {
    opcode->kind = OPCODE_KIND_0x90_SUB_A_B;
    break;
  }
  case 0x91: {
    opcode->kind = OPCODE_KIND_0x91_SUB_A_C;
    break;
  }
  case 0x92: {
    opcode->kind = OPCODE_KIND_0x92_SUB_A_D;
    break;
  }
  case 0x93: {
    opcode->kind = OPCODE_KIND_0x93_SUB_A_E;
    break;
  }
  case 0x94: {
    opcode->kind = OPCODE_KIND_0x94_SUB_A_H;
    break;
  }
  case 0x95: {
    opcode->kind = OPCODE_KIND_0x95_SUB_A_L;
    break;
  }
  case 0x96: {
    opcode->kind = OPCODE_KIND_0x96_SUB_A_pHL;
    break;
  }
  case 0x97: {
    opcode->kind = OPCODE_KIND_0x97_SUB_A_A;
    break;
  }
  case 0x98: {
    opcode->kind = OPCODE_KIND_0x98_SBC_A_B;
    break;
  }
  case 0x99: {
    opcode->kind = OPCODE_KIND_0x99_SBC_A_C;
    break;
  }
  case 0x9a: {
    opcode->kind = OPCODE_KIND_0x9a_SBC_A_D;
    break;
  }
  case 0x9b: {
    opcode->kind = OPCODE_KIND_0x9b_SBC_A_E;
    break;
  }
  case 0x9c: {
    opcode->kind = OPCODE_KIND_0x9c_SBC_A_H;
    break;
  }
  case 0x9d: {
    opcode->kind = OPCODE_KIND_0x9d_SBC_A_L;
    break;
  }
  case 0x9e: {
    opcode->kind = OPCODE_KIND_0x9e_SBC_A_pHL;
    break;
  }
  case 0x9f: {
    opcode->kind = OPCODE_KIND_0x9f_SBC_A_A;
    break;
  }
  case 0xa0: {
    opcode->kind = OPCODE_KIND_0xa0_AND_A_B;
    break;
  }
  case 0xa1: {
    opcode->kind = OPCODE_KIND_0xa1_AND_A_C;
    break;
  }
  case 0xa2: {
    opcode->kind = OPCODE_KIND_0xa2_AND_A_D;
    break;
  }
  case 0xa3: {
    opcode->kind = OPCODE_KIND_0xa3_AND_A_E;
    break;
  }
  case 0xa4: {
    opcode->kind = OPCODE_KIND_0xa4_AND_A_H;
    break;
  }
  case 0xa5: {
    opcode->kind = OPCODE_KIND_0xa5_AND_A_L;
    break;
  }
  case 0xa6: {
    opcode->kind = OPCODE_KIND_0xa6_AND_A_pHL;
    break;
  }
  case 0xa7: {
    opcode->kind = OPCODE_KIND_0xa7_AND_A_A;
    break;
  }
  case 0xa8: {
    opcode->kind = OPCODE_KIND_0xa8_XOR_A_B;
    break;
  }
  case 0xa9: {
    opcode->kind = OPCODE_KIND_0xa9_XOR_A_C;
    break;
  }
  case 0xaa: {
    opcode->kind = OPCODE_KIND_0xaa_XOR_A_D;
    break;
  }
  case 0xab: {
    opcode->kind = OPCODE_KIND_0xab_XOR_A_E;
    break;
  }
  case 0xac: {
    opcode->kind = OPCODE_KIND_0xac_XOR_A_H;
    break;
  }
  case 0xad: {
    opcode->kind = OPCODE_KIND_0xad_XOR_A_L;
    break;
  }
  case 0xae: {
    opcode->kind = OPCODE_KIND_0xae_XOR_A_pHL;
    break;
  }
  case 0xaf: {
    opcode->kind = OPCODE_KIND_0xaf_XOR_A_A;
    break;
  }
  case 0xb0: {
    opcode->kind = OPCODE_KIND_0xb0_OR_A_B;
    break;
  }
  case 0xb1: {
    opcode->kind = OPCODE_KIND_0xb1_OR_A_C;
    break;
  }
  case 0xb2: {
    opcode->kind = OPCODE_KIND_0xb2_OR_A_D;
    break;
  }
  case 0xb3: {
    opcode->kind = OPCODE_KIND_0xb3_OR_A_E;
    break;
  }
  case 0xb4: {
    opcode->kind = OPCODE_KIND_0xb4_OR_A_H;
    break;
  }
  case 0xb5: {
    opcode->kind = OPCODE_KIND_0xb5_OR_A_L;
    break;
  }
  case 0xb6: {
    opcode->kind = OPCODE_KIND_0xb6_OR_A_pHL;
    break;
  }
  case 0xb7: {
    opcode->kind = OPCODE_KIND_0xb7_OR_A_A;
    break;
  }
  case 0xb8: {
    opcode->kind = OPCODE_KIND_0xb8_CP_A_B;
    break;
  }
  case 0xb9: {
    opcode->kind = OPCODE_KIND_0xb9_CP_A_C;
    break;
  }
  case 0xba: {
    opcode->kind = OPCODE_KIND_0xba_CP_A_D;
    break;
  }
  case 0xbb: {
    opcode->kind = OPCODE_KIND_0xbb_CP_A_E;
    break;
  }
  case 0xbc: {
    opcode->kind = OPCODE_KIND_0xbc_CP_A_H;
    break;
  }
  case 0xbd: {
    opcode->kind = OPCODE_KIND_0xbd_CP_A_L;
    break;
  }
  case 0xbe: {
    opcode->kind = OPCODE_KIND_0xbe_CP_A_pHL;
    break;
  }
  case 0xbf: {
    opcode->kind = OPCODE_KIND_0xbf_CP_A_A;
    break;
  }
  case 0xc6: {
    opcode->kind = OPCODE_KIND_0xc6_ADD_A_n8;
    opcode->n8 = read_u8(emu);
    break;
  }
  case 0xce: {
    opcode->kind = OPCODE_KIND_0xce_ADC_A_n8;
    opcode->n8 = read_u8(emu);
    break;
  }
  case 0xd6: {
    opcode->kind = OPCODE_KIND_0xd6_SUB_A_n8;
    opcode->n8 = read_u8(emu);
    break;
  }
  case 0xde: {
    opcode->kind = OPCODE_KIND_0xde_SBC_A_n8;
    opcode->n8 = read_u8(emu);
    break;
  }
  case 0xe6: {
    opcode->kind = OPCODE_KIND_0xe6_AND_A_n8;
    opcode->n8 = read_u8(emu);
    break;
  }
  case 0xee: {
    opcode->kind = OPCODE_KIND_0xee_XOR_A_n8;
    opcode->n8 = read_u8(emu);
    break;
  }
  case 0xf6: {
    opcode->kind = OPCODE_KIND_0xf6_OR_A_n8;
    opcode->n8 = read_u8(emu);
    break;
  }
  case 0xfe: {
    opcode->kind = OPCODE_KIND_0xfe_CP_A_n8;
    opcode->n8 = read_u8(emu);
    break;
  }
  case 0x07: {
    opcode->kind = OPCODE_KIND_0x07_RLCA;
    break;
  }
  case 0x17: {
    opcode->kind = OPCODE_KIND_0x17_RLA;
    break;
  }
  case 0x0f: {
    opcode->kind = OPCODE_KIND_0x0f_RRCA;
    break;
  }
  case 0x1f: {
    opcode->kind = OPCODE_KIND_0x1f_RRA;
    break;
  }
  case 0x09: {
    opcode->kind = OPCODE_KIND_0x09_ADD_HL_BC;
    break;
  }
  case 0x19: {
    opcode->kind = OPCODE_KIND_0x19_ADD_HL_DE;
    break;
  }
  case 0x29: {
    opcode->kind = OPCODE_KIND_0x29_ADD_HL_HL;
    break;
  }
  case 0x39: {
    opcode->kind = OPCODE_KIND_0x39_ADD_HL_SP;
    break;
  }
  case 0xe9: {
    opcode->kind = OPCODE_KIND_0xe9_JP_HL;
    break;
  }
  case 0xca: {
    opcode->kind = OPCODE_KIND_0xca_JP_Z_a16;
    opcode->a16 = read_u16(emu);
    break;
  }
  case 0xda: {
    opcode->kind = OPCODE_KIND_0xda_JP_C_a16;
    opcode->a16 = read_u16(emu);
    break;
  }
  case 0xc2: {
    opcode->kind = OPCODE_KIND_0xc2_JP_NZ_a16;
    opcode->a16 = read_u16(emu);
    break;
  }
  case 0xd2: {
    opcode->kind = OPCODE_KIND_0xd2_JP_NC_a16;
    opcode->a16 = read_u16(emu);
    break;
  }
  case 0x27: {
    opcode->kind = OPCODE_KIND_0x27_DAA;
    break;
  }
  case 0x08: {
    opcode->kind = OPCODE_KIND_0x08_LD_pa16_SP;
    opcode->a16 = read_u16(emu);
    break;
  }
  case 0xf9: {
    opcode->kind = OPCODE_KIND_0xf9_LD_SP_HL;
    break;
  }
  case 0xe8: {
    opcode->kind = OPCODE_KIND_0xe8_ADD_SP_e8;
    opcode->e8 = read_i8(emu);
    break;
  }
  case 0xf8: {
    opcode->kind = OPCODE_KIND_0xf8_LD_HL_SP_e8;
    opcode->e8 = read_i8(emu);
    break;
  }
  case 0xd9: {
    opcode->kind = OPCODE_KIND_0xd9_RETI;
    break;
  }
  case 0xc7: {
    opcode->kind = OPCODE_KIND_0xc7_RST_0x00;
    break;
  }
  case 0xd7: {
    opcode->kind = OPCODE_KIND_0xd7_RST_0x10;
    break;
  }
  case 0xe7: {
    opcode->kind = OPCODE_KIND_0xe7_RST_0x20;
    break;
  }
  case 0xf7: {
    opcode->kind = OPCODE_KIND_0xf7_RST_0x30;
    break;
  }
  case 0xcf: {
    opcode->kind = OPCODE_KIND_0xcf_RST_0x08;
    break;
  }
  case 0xdf: {
    opcode->kind = OPCODE_KIND_0xdf_RST_0x18;
    break;
  }
  case 0xef: {
    opcode->kind = OPCODE_KIND_0xef_RST_0x28;
    break;
  }
  case 0xff: {
    opcode->kind = OPCODE_KIND_0xff_RST_0x38;
    break;
  }
  case 0x2f: {
    opcode->kind = OPCODE_KIND_0x2f_CPL;
    break;
  }
  case 0x3f: {
    opcode->kind = OPCODE_KIND_0x3f_CCF;
    break;
  }
  case 0x37: {
    opcode->kind = OPCODE_KIND_0x37_SCF;
    break;
  }
  case 0x76: {
    opcode->kind = OPCODE_KIND_0x76_HALT;
    break;
  }
  case 0xcb: {
    opcode->kind = OPCODE_KIND_0xcb_PREFIX;
    uint8_t prefix = read_u8(emu);
    switch (prefix) {
    case 0x00: {
      opcode->prefix = OPCODE_PREFIX_0x00_RLC_B;
      break;
    }
    case 0x01: {
      opcode->prefix = OPCODE_PREFIX_0x01_RLC_C;
      break;
    }
    case 0x02: {
      opcode->prefix = OPCODE_PREFIX_0x02_RLC_D;
      break;
    }
    case 0x03: {
      opcode->prefix = OPCODE_PREFIX_0x03_RLC_E;
      break;
    }
    case 0x04: {
      opcode->prefix = OPCODE_PREFIX_0x04_RLC_H;
      break;
    }
    case 0x05: {
      opcode->prefix = OPCODE_PREFIX_0x05_RLC_L;
      break;
    }
    case 0x06: {
      opcode->prefix = OPCODE_PREFIX_0x06_RLC_pHL;
      break;
    }
    case 0x07: {
      opcode->prefix = OPCODE_PREFIX_0x07_RLC_A;
      break;
    }
    case 0x08: {
      opcode->prefix = OPCODE_PREFIX_0x08_RRC_B;
      break;
    }
    case 0x09: {
      opcode->prefix = OPCODE_PREFIX_0x09_RRC_C;
      break;
    }
    case 0x0a: {
      opcode->prefix = OPCODE_PREFIX_0x0a_RRC_D;
      break;
    }
    case 0x0b: {
      opcode->prefix = OPCODE_PREFIX_0x0b_RRC_E;
      break;
    }
    case 0x0c: {
      opcode->prefix = OPCODE_PREFIX_0x0c_RRC_H;
      break;
    }
    case 0x0d: {
      opcode->prefix = OPCODE_PREFIX_0x0d_RRC_L;
      break;
    }
    case 0x0e: {
      opcode->prefix = OPCODE_PREFIX_0x0e_RRC_pHL;
      break;
    }
    case 0x0f: {
      opcode->prefix = OPCODE_PREFIX_0x0f_RRC_A;
      break;
    }
    case 0x10: {
      opcode->prefix = OPCODE_PREFIX_0x10_RL_B;
      break;
    }
    case 0x11: {
      opcode->prefix = OPCODE_PREFIX_0x11_RL_C;
      break;
    }
    case 0x12: {
      opcode->prefix = OPCODE_PREFIX_0x12_RL_D;
      break;
    }
    case 0x13: {
      opcode->prefix = OPCODE_PREFIX_0x13_RL_E;
      break;
    }
    case 0x14: {
      opcode->prefix = OPCODE_PREFIX_0x14_RL_H;
      break;
    }
    case 0x15: {
      opcode->prefix = OPCODE_PREFIX_0x15_RL_L;
      break;
    }
    case 0x16: {
      opcode->prefix = OPCODE_PREFIX_0x16_RL_pHL;
      break;
    }
    case 0x17: {
      opcode->prefix = OPCODE_PREFIX_0x17_RL_A;
      break;
    }
    case 0x18: {
      opcode->prefix = OPCODE_PREFIX_0x18_RR_B;
      break;
    }
    case 0x19: {
      opcode->prefix = OPCODE_PREFIX_0x19_RR_C;
      break;
    }
    case 0x1a: {
      opcode->prefix = OPCODE_PREFIX_0x1a_RR_D;
      break;
    }
    case 0x1b: {
      opcode->prefix = OPCODE_PREFIX_0x1b_RR_E;
      break;
    }
    case 0x1c: {
      opcode->prefix = OPCODE_PREFIX_0x1c_RR_H;
      break;
    }
    case 0x1d: {
      opcode->prefix = OPCODE_PREFIX_0x1d_RR_L;
      break;
    }
    case 0x1e: {
      opcode->prefix = OPCODE_PREFIX_0x1e_RR_pHL;
      break;
    }
    case 0x1f: {
      opcode->prefix = OPCODE_PREFIX_0x1f_RR_A;
      break;
    }
    case 0x20: {
      opcode->prefix = OPCODE_PREFIX_0x20_SLA_B;
      break;
    }
    case 0x21: {
      opcode->prefix = OPCODE_PREFIX_0x21_SLA_C;
      break;
    }
    case 0x22: {
      opcode->prefix = OPCODE_PREFIX_0x22_SLA_D;
      break;
    }
    case 0x23: {
      opcode->prefix = OPCODE_PREFIX_0x23_SLA_E;
      break;
    }
    case 0x24: {
      opcode->prefix = OPCODE_PREFIX_0x24_SLA_H;
      break;
    }
    case 0x25: {
      opcode->prefix = OPCODE_PREFIX_0x25_SLA_L;
      break;
    }
    case 0x26: {
      opcode->prefix = OPCODE_PREFIX_0x26_SLA_pHL;
      break;
    }
    case 0x27: {
      opcode->prefix = OPCODE_PREFIX_0x27_SLA_A;
      break;
    }
    case 0x28: {
      opcode->prefix = OPCODE_PREFIX_0x28_SRA_B;
      break;
    }
    case 0x29: {
      opcode->prefix = OPCODE_PREFIX_0x29_SRA_C;
      break;
    }
    case 0x2a: {
      opcode->prefix = OPCODE_PREFIX_0x2a_SRA_D;
      break;
    }
    case 0x2b: {
      opcode->prefix = OPCODE_PREFIX_0x2b_SRA_E;
      break;
    }
    case 0x2c: {
      opcode->prefix = OPCODE_PREFIX_0x2c_SRA_H;
      break;
    }
    case 0x2d: {
      opcode->prefix = OPCODE_PREFIX_0x2d_SRA_L;
      break;
    }
    case 0x2e: {
      opcode->prefix = OPCODE_PREFIX_0x2e_SRA_pHL;
      break;
    }
    case 0x2f: {
      opcode->prefix = OPCODE_PREFIX_0x2f_SRA_A;
      break;
    }
    case 0x30: {
      opcode->prefix = OPCODE_PREFIX_0x30_SWAP_B;
      break;
    }
    case 0x31: {
      opcode->prefix = OPCODE_PREFIX_0x31_SWAP_C;
      break;
    }
    case 0x32: {
      opcode->prefix = OPCODE_PREFIX_0x32_SWAP_D;
      break;
    }
    case 0x33: {
      opcode->prefix = OPCODE_PREFIX_0x33_SWAP_E;
      break;
    }
    case 0x34: {
      opcode->prefix = OPCODE_PREFIX_0x34_SWAP_H;
      break;
    }
    case 0x35: {
      opcode->prefix = OPCODE_PREFIX_0x35_SWAP_L;
      break;
    }
    case 0x36: {
      opcode->prefix = OPCODE_PREFIX_0x36_SWAP_pHL;
      break;
    }
    case 0x37: {
      opcode->prefix = OPCODE_PREFIX_0x37_SWAP_A;
      break;
    }
    case 0x38: {
      opcode->prefix = OPCODE_PREFIX_0x38_SRL_B;
      break;
    }
    case 0x39: {
      opcode->prefix = OPCODE_PREFIX_0x39_SRL_C;
      break;
    }
    case 0x3a: {
      opcode->prefix = OPCODE_PREFIX_0x3a_SRL_D;
      break;
    }
    case 0x3b: {
      opcode->prefix = OPCODE_PREFIX_0x3b_SRL_E;
      break;
    }
    case 0x3c: {
      opcode->prefix = OPCODE_PREFIX_0x3c_SRL_H;
      break;
    }
    case 0x3d: {
      opcode->prefix = OPCODE_PREFIX_0x3d_SRL_L;
      break;
    }
    case 0x3e: {
      opcode->prefix = OPCODE_PREFIX_0x3e_SRL_pHL;
      break;
    }
    case 0x3f: {
      opcode->prefix = OPCODE_PREFIX_0x3f_SRL_A;
      break;
    }
    case 0x40: {
      opcode->prefix = OPCODE_PREFIX_0x40_BIT_0_B;
      break;
    }
    case 0x41: {
      opcode->prefix = OPCODE_PREFIX_0x41_BIT_0_C;
      break;
    }
    case 0x42: {
      opcode->prefix = OPCODE_PREFIX_0x42_BIT_0_D;
      break;
    }
    case 0x43: {
      opcode->prefix = OPCODE_PREFIX_0x43_BIT_0_E;
      break;
    }
    case 0x44: {
      opcode->prefix = OPCODE_PREFIX_0x44_BIT_0_H;
      break;
    }
    case 0x45: {
      opcode->prefix = OPCODE_PREFIX_0x45_BIT_0_L;
      break;
    }
    case 0x46: {
      opcode->prefix = OPCODE_PREFIX_0x46_BIT_0_pHL;
      break;
    }
    case 0x47: {
      opcode->prefix = OPCODE_PREFIX_0x47_BIT_0_A;
      break;
    }
    case 0x48: {
      opcode->prefix = OPCODE_PREFIX_0x48_BIT_1_B;
      break;
    }
    case 0x49: {
      opcode->prefix = OPCODE_PREFIX_0x49_BIT_1_C;
      break;
    }
    case 0x4a: {
      opcode->prefix = OPCODE_PREFIX_0x4a_BIT_1_D;
      break;
    }
    case 0x4b: {
      opcode->prefix = OPCODE_PREFIX_0x4b_BIT_1_E;
      break;
    }
    case 0x4c: {
      opcode->prefix = OPCODE_PREFIX_0x4c_BIT_1_H;
      break;
    }
    case 0x4d: {
      opcode->prefix = OPCODE_PREFIX_0x4d_BIT_1_L;
      break;
    }
    case 0x4e: {
      opcode->prefix = OPCODE_PREFIX_0x4e_BIT_1_pHL;
      break;
    }
    case 0x4f: {
      opcode->prefix = OPCODE_PREFIX_0x4f_BIT_1_A;
      break;
    }
    case 0x50: {
      opcode->prefix = OPCODE_PREFIX_0x50_BIT_2_B;
      break;
    }
    case 0x51: {
      opcode->prefix = OPCODE_PREFIX_0x51_BIT_2_C;
      break;
    }
    case 0x52: {
      opcode->prefix = OPCODE_PREFIX_0x52_BIT_2_D;
      break;
    }
    case 0x53: {
      opcode->prefix = OPCODE_PREFIX_0x53_BIT_2_E;
      break;
    }
    case 0x54: {
      opcode->prefix = OPCODE_PREFIX_0x54_BIT_2_H;
      break;
    }
    case 0x55: {
      opcode->prefix = OPCODE_PREFIX_0x55_BIT_2_L;
      break;
    }
    case 0x56: {
      opcode->prefix = OPCODE_PREFIX_0x56_BIT_2_pHL;
      break;
    }
    case 0x57: {
      opcode->prefix = OPCODE_PREFIX_0x57_BIT_2_A;
      break;
    }
    case 0x58: {
      opcode->prefix = OPCODE_PREFIX_0x58_BIT_3_B;
      break;
    }
    case 0x59: {
      opcode->prefix = OPCODE_PREFIX_0x59_BIT_3_C;
      break;
    }
    case 0x5a: {
      opcode->prefix = OPCODE_PREFIX_0x5a_BIT_3_D;
      break;
    }
    case 0x5b: {
      opcode->prefix = OPCODE_PREFIX_0x5b_BIT_3_E;
      break;
    }
    case 0x5c: {
      opcode->prefix = OPCODE_PREFIX_0x5c_BIT_3_H;
      break;
    }
    case 0x5d: {
      opcode->prefix = OPCODE_PREFIX_0x5d_BIT_3_L;
      break;
    }
    case 0x5e: {
      opcode->prefix = OPCODE_PREFIX_0x5e_BIT_3_pHL;
      break;
    }
    case 0x5f: {
      opcode->prefix = OPCODE_PREFIX_0x5f_BIT_3_A;
      break;
    }
    case 0x60: {
      opcode->prefix = OPCODE_PREFIX_0x60_BIT_4_B;
      break;
    }
    case 0x61: {
      opcode->prefix = OPCODE_PREFIX_0x61_BIT_4_C;
      break;
    }
    case 0x62: {
      opcode->prefix = OPCODE_PREFIX_0x62_BIT_4_D;
      break;
    }
    case 0x63: {
      opcode->prefix = OPCODE_PREFIX_0x63_BIT_4_E;
      break;
    }
    case 0x64: {
      opcode->prefix = OPCODE_PREFIX_0x64_BIT_4_H;
      break;
    }
    case 0x65: {
      opcode->prefix = OPCODE_PREFIX_0x65_BIT_4_L;
      break;
    }
    case 0x66: {
      opcode->prefix = OPCODE_PREFIX_0x66_BIT_4_pHL;
      break;
    }
    case 0x67: {
      opcode->prefix = OPCODE_PREFIX_0x67_BIT_4_A;
      break;
    }
    case 0x68: {
      opcode->prefix = OPCODE_PREFIX_0x68_BIT_5_B;
      break;
    }
    case 0x69: {
      opcode->prefix = OPCODE_PREFIX_0x69_BIT_5_C;
      break;
    }
    case 0x6a: {
      opcode->prefix = OPCODE_PREFIX_0x6a_BIT_5_D;
      break;
    }
    case 0x6b: {
      opcode->prefix = OPCODE_PREFIX_0x6b_BIT_5_E;
      break;
    }
    case 0x6c: {
      opcode->prefix = OPCODE_PREFIX_0x6c_BIT_5_H;
      break;
    }
    case 0x6d: {
      opcode->prefix = OPCODE_PREFIX_0x6d_BIT_5_L;
      break;
    }
    case 0x6e: {
      opcode->prefix = OPCODE_PREFIX_0x6e_BIT_5_pHL;
      break;
    }
    case 0x6f: {
      opcode->prefix = OPCODE_PREFIX_0x6f_BIT_5_A;
      break;
    }
    case 0x70: {
      opcode->prefix = OPCODE_PREFIX_0x70_BIT_6_B;
      break;
    }
    case 0x71: {
      opcode->prefix = OPCODE_PREFIX_0x71_BIT_6_C;
      break;
    }
    case 0x72: {
      opcode->prefix = OPCODE_PREFIX_0x72_BIT_6_D;
      break;
    }
    case 0x73: {
      opcode->prefix = OPCODE_PREFIX_0x73_BIT_6_E;
      break;
    }
    case 0x74: {
      opcode->prefix = OPCODE_PREFIX_0x74_BIT_6_H;
      break;
    }
    case 0x75: {
      opcode->prefix = OPCODE_PREFIX_0x75_BIT_6_L;
      break;
    }
    case 0x76: {
      opcode->prefix = OPCODE_PREFIX_0x76_BIT_6_pHL;
      break;
    }
    case 0x77: {
      opcode->prefix = OPCODE_PREFIX_0x77_BIT_6_A;
      break;
    }
    case 0x78: {
      opcode->prefix = OPCODE_PREFIX_0x78_BIT_7_B;
      break;
    }
    case 0x79: {
      opcode->prefix = OPCODE_PREFIX_0x79_BIT_7_C;
      break;
    }
    case 0x7a: {
      opcode->prefix = OPCODE_PREFIX_0x7a_BIT_7_D;
      break;
    }
    case 0x7b: {
      opcode->prefix = OPCODE_PREFIX_0x7b_BIT_7_E;
      break;
    }
    case 0x7c: {
      opcode->prefix = OPCODE_PREFIX_0x7c_BIT_7_H;
      break;
    }
    case 0x7d: {
      opcode->prefix = OPCODE_PREFIX_0x7d_BIT_7_L;
      break;
    }
    case 0x7e: {
      opcode->prefix = OPCODE_PREFIX_0x7e_BIT_7_pHL;
      break;
    }
    case 0x7f: {
      opcode->prefix = OPCODE_PREFIX_0x7f_BIT_7_A;
      break;
    }
    case 0x80: {
      opcode->prefix = OPCODE_PREFIX_0x80_RES_0_B;
      break;
    }
    case 0x81: {
      opcode->prefix = OPCODE_PREFIX_0x81_RES_0_C;
      break;
    }
    case 0x82: {
      opcode->prefix = OPCODE_PREFIX_0x82_RES_0_D;
      break;
    }
    case 0x83: {
      opcode->prefix = OPCODE_PREFIX_0x83_RES_0_E;
      break;
    }
    case 0x84: {
      opcode->prefix = OPCODE_PREFIX_0x84_RES_0_H;
      break;
    }
    case 0x85: {
      opcode->prefix = OPCODE_PREFIX_0x85_RES_0_L;
      break;
    }
    case 0x86: {
      opcode->prefix = OPCODE_PREFIX_0x86_RES_0_pHL;
      break;
    }
    case 0x87: {
      opcode->prefix = OPCODE_PREFIX_0x87_RES_0_A;
      break;
    }
    case 0x88: {
      opcode->prefix = OPCODE_PREFIX_0x88_RES_1_B;
      break;
    }
    case 0x89: {
      opcode->prefix = OPCODE_PREFIX_0x89_RES_1_C;
      break;
    }
    case 0x8a: {
      opcode->prefix = OPCODE_PREFIX_0x8a_RES_1_D;
      break;
    }
    case 0x8b: {
      opcode->prefix = OPCODE_PREFIX_0x8b_RES_1_E;
      break;
    }
    case 0x8c: {
      opcode->prefix = OPCODE_PREFIX_0x8c_RES_1_H;
      break;
    }
    case 0x8d: {
      opcode->prefix = OPCODE_PREFIX_0x8d_RES_1_L;
      break;
    }
    case 0x8e: {
      opcode->prefix = OPCODE_PREFIX_0x8e_RES_1_pHL;
      break;
    }
    case 0x8f: {
      opcode->prefix = OPCODE_PREFIX_0x8f_RES_1_A;
      break;
    }
    case 0x90: {
      opcode->prefix = OPCODE_PREFIX_0x90_RES_2_B;
      break;
    }
    case 0x91: {
      opcode->prefix = OPCODE_PREFIX_0x91_RES_2_C;
      break;
    }
    case 0x92: {
      opcode->prefix = OPCODE_PREFIX_0x92_RES_2_D;
      break;
    }
    case 0x93: {
      opcode->prefix = OPCODE_PREFIX_0x93_RES_2_E;
      break;
    }
    case 0x94: {
      opcode->prefix = OPCODE_PREFIX_0x94_RES_2_H;
      break;
    }
    case 0x95: {
      opcode->prefix = OPCODE_PREFIX_0x95_RES_2_L;
      break;
    }
    case 0x96: {
      opcode->prefix = OPCODE_PREFIX_0x96_RES_2_pHL;
      break;
    }
    case 0x97: {
      opcode->prefix = OPCODE_PREFIX_0x97_RES_2_A;
      break;
    }
    case 0x98: {
      opcode->prefix = OPCODE_PREFIX_0x98_RES_3_B;
      break;
    }
    case 0x99: {
      opcode->prefix = OPCODE_PREFIX_0x99_RES_3_C;
      break;
    }
    case 0x9a: {
      opcode->prefix = OPCODE_PREFIX_0x9a_RES_3_D;
      break;
    }
    case 0x9b: {
      opcode->prefix = OPCODE_PREFIX_0x9b_RES_3_E;
      break;
    }
    case 0x9c: {
      opcode->prefix = OPCODE_PREFIX_0x9c_RES_3_H;
      break;
    }
    case 0x9d: {
      opcode->prefix = OPCODE_PREFIX_0x9d_RES_3_L;
      break;
    }
    case 0x9e: {
      opcode->prefix = OPCODE_PREFIX_0x9e_RES_3_pHL;
      break;
    }
    case 0x9f: {
      opcode->prefix = OPCODE_PREFIX_0x9f_RES_3_A;
      break;
    }
    case 0xa0: {
      opcode->prefix = OPCODE_PREFIX_0xa0_RES_4_B;
      break;
    }
    case 0xa1: {
      opcode->prefix = OPCODE_PREFIX_0xa1_RES_4_C;
      break;
    }
    case 0xa2: {
      opcode->prefix = OPCODE_PREFIX_0xa2_RES_4_D;
      break;
    }
    case 0xa3: {
      opcode->prefix = OPCODE_PREFIX_0xa3_RES_4_E;
      break;
    }
    case 0xa4: {
      opcode->prefix = OPCODE_PREFIX_0xa4_RES_4_H;
      break;
    }
    case 0xa5: {
      opcode->prefix = OPCODE_PREFIX_0xa5_RES_4_L;
      break;
    }
    case 0xa6: {
      opcode->prefix = OPCODE_PREFIX_0xa6_RES_4_pHL;
      break;
    }
    case 0xa7: {
      opcode->prefix = OPCODE_PREFIX_0xa7_RES_4_A;
      break;
    }
    case 0xa8: {
      opcode->prefix = OPCODE_PREFIX_0xa8_RES_5_B;
      break;
    }
    case 0xa9: {
      opcode->prefix = OPCODE_PREFIX_0xa9_RES_5_C;
      break;
    }
    case 0xaa: {
      opcode->prefix = OPCODE_PREFIX_0xaa_RES_5_D;
      break;
    }
    case 0xab: {
      opcode->prefix = OPCODE_PREFIX_0xab_RES_5_E;
      break;
    }
    case 0xac: {
      opcode->prefix = OPCODE_PREFIX_0xac_RES_5_H;
      break;
    }
    case 0xad: {
      opcode->prefix = OPCODE_PREFIX_0xad_RES_5_L;
      break;
    }
    case 0xae: {
      opcode->prefix = OPCODE_PREFIX_0xae_RES_5_pHL;
      break;
    }
    case 0xaf: {
      opcode->prefix = OPCODE_PREFIX_0xaf_RES_5_A;
      break;
    }
    case 0xb0: {
      opcode->prefix = OPCODE_PREFIX_0xb0_RES_6_B;
      break;
    }
    case 0xb1: {
      opcode->prefix = OPCODE_PREFIX_0xb1_RES_6_C;
      break;
    }
    case 0xb2: {
      opcode->prefix = OPCODE_PREFIX_0xb2_RES_6_D;
      break;
    }
    case 0xb3: {
      opcode->prefix = OPCODE_PREFIX_0xb3_RES_6_E;
      break;
    }
    case 0xb4: {
      opcode->prefix = OPCODE_PREFIX_0xb4_RES_6_H;
      break;
    }
    case 0xb5: {
      opcode->prefix = OPCODE_PREFIX_0xb5_RES_6_L;
      break;
    }
    case 0xb6: {
      opcode->prefix = OPCODE_PREFIX_0xb6_RES_6_pHL;
      break;
    }
    case 0xb7: {
      opcode->prefix = OPCODE_PREFIX_0xb7_RES_6_A;
      break;
    }
    case 0xb8: {
      opcode->prefix = OPCODE_PREFIX_0xb8_RES_7_B;
      break;
    }
    case 0xb9: {
      opcode->prefix = OPCODE_PREFIX_0xb9_RES_7_C;
      break;
    }
    case 0xba: {
      opcode->prefix = OPCODE_PREFIX_0xba_RES_7_D;
      break;
    }
    case 0xbb: {
      opcode->prefix = OPCODE_PREFIX_0xbb_RES_7_E;
      break;
    }
    case 0xbc: {
      opcode->prefix = OPCODE_PREFIX_0xbc_RES_7_H;
      break;
    }
    case 0xbd: {
      opcode->prefix = OPCODE_PREFIX_0xbd_RES_7_L;
      break;
    }
    case 0xbe: {
      opcode->prefix = OPCODE_PREFIX_0xbe_RES_7_pHL;
      break;
    }
    case 0xbf: {
      opcode->prefix = OPCODE_PREFIX_0xbf_RES_7_A;
      break;
    }
    case 0xc0: {
      opcode->prefix = OPCODE_PREFIX_0xc0_SET_0_B;
      break;
    }
    case 0xc1: {
      opcode->prefix = OPCODE_PREFIX_0xc1_SET_0_C;
      break;
    }
    case 0xc2: {
      opcode->prefix = OPCODE_PREFIX_0xc2_SET_0_D;
      break;
    }
    case 0xc3: {
      opcode->prefix = OPCODE_PREFIX_0xc3_SET_0_E;
      break;
    }
    case 0xc4: {
      opcode->prefix = OPCODE_PREFIX_0xc4_SET_0_H;
      break;
    }
    case 0xc5: {
      opcode->prefix = OPCODE_PREFIX_0xc5_SET_0_L;
      break;
    }
    case 0xc6: {
      opcode->prefix = OPCODE_PREFIX_0xc6_SET_0_pHL;
      break;
    }
    case 0xc7: {
      opcode->prefix = OPCODE_PREFIX_0xc7_SET_0_A;
      break;
    }
    case 0xc8: {
      opcode->prefix = OPCODE_PREFIX_0xc8_SET_1_B;
      break;
    }
    case 0xc9: {
      opcode->prefix = OPCODE_PREFIX_0xc9_SET_1_C;
      break;
    }
    case 0xca: {
      opcode->prefix = OPCODE_PREFIX_0xca_SET_1_D;
      break;
    }
    case 0xcb: {
      opcode->prefix = OPCODE_PREFIX_0xcb_SET_1_E;
      break;
    }
    case 0xcc: {
      opcode->prefix = OPCODE_PREFIX_0xcc_SET_1_H;
      break;
    }
    case 0xcd: {
      opcode->prefix = OPCODE_PREFIX_0xcd_SET_1_L;
      break;
    }
    case 0xce: {
      opcode->prefix = OPCODE_PREFIX_0xce_SET_1_pHL;
      break;
    }
    case 0xcf: {
      opcode->prefix = OPCODE_PREFIX_0xcf_SET_1_A;
      break;
    }
    case 0xd0: {
      opcode->prefix = OPCODE_PREFIX_0xd0_SET_2_B;
      break;
    }
    case 0xd1: {
      opcode->prefix = OPCODE_PREFIX_0xd1_SET_2_C;
      break;
    }
    case 0xd2: {
      opcode->prefix = OPCODE_PREFIX_0xd2_SET_2_D;
      break;
    }
    case 0xd3: {
      opcode->prefix = OPCODE_PREFIX_0xd3_SET_2_E;
      break;
    }
    case 0xd4: {
      opcode->prefix = OPCODE_PREFIX_0xd4_SET_2_H;
      break;
    }
    case 0xd5: {
      opcode->prefix = OPCODE_PREFIX_0xd5_SET_2_L;
      break;
    }
    case 0xd6: {
      opcode->prefix = OPCODE_PREFIX_0xd6_SET_2_pHL;
      break;
    }
    case 0xd7: {
      opcode->prefix = OPCODE_PREFIX_0xd7_SET_2_A;
      break;
    }
    case 0xd8: {
      opcode->prefix = OPCODE_PREFIX_0xd8_SET_3_B;
      break;
    }
    case 0xd9: {
      opcode->prefix = OPCODE_PREFIX_0xd9_SET_3_C;
      break;
    }
    case 0xda: {
      opcode->prefix = OPCODE_PREFIX_0xda_SET_3_D;
      break;
    }
    case 0xdb: {
      opcode->prefix = OPCODE_PREFIX_0xdb_SET_3_E;
      break;
    }
    case 0xdc: {
      opcode->prefix = OPCODE_PREFIX_0xdc_SET_3_H;
      break;
    }
    case 0xdd: {
      opcode->prefix = OPCODE_PREFIX_0xdd_SET_3_L;
      break;
    }
    case 0xde: {
      opcode->prefix = OPCODE_PREFIX_0xde_SET_3_pHL;
      break;
    }
    case 0xdf: {
      opcode->prefix = OPCODE_PREFIX_0xdf_SET_3_A;
      break;
    }
    case 0xe0: {
      opcode->prefix = OPCODE_PREFIX_0xe0_SET_4_B;
      break;
    }
    case 0xe1: {
      opcode->prefix = OPCODE_PREFIX_0xe1_SET_4_C;
      break;
    }
    case 0xe2: {
      opcode->prefix = OPCODE_PREFIX_0xe2_SET_4_D;
      break;
    }
    case 0xe3: {
      opcode->prefix = OPCODE_PREFIX_0xe3_SET_4_E;
      break;
    }
    case 0xe4: {
      opcode->prefix = OPCODE_PREFIX_0xe4_SET_4_H;
      break;
    }
    case 0xe5: {
      opcode->prefix = OPCODE_PREFIX_0xe5_SET_4_L;
      break;
    }
    case 0xe6: {
      opcode->prefix = OPCODE_PREFIX_0xe6_SET_4_pHL;
      break;
    }
    case 0xe7: {
      opcode->prefix = OPCODE_PREFIX_0xe7_SET_4_A;
      break;
    }
    case 0xe8: {
      opcode->prefix = OPCODE_PREFIX_0xe8_SET_5_B;
      break;
    }
    case 0xe9: {
      opcode->prefix = OPCODE_PREFIX_0xe9_SET_5_C;
      break;
    }
    case 0xea: {
      opcode->prefix = OPCODE_PREFIX_0xea_SET_5_D;
      break;
    }
    case 0xeb: {
      opcode->prefix = OPCODE_PREFIX_0xeb_SET_5_E;
      break;
    }
    case 0xec: {
      opcode->prefix = OPCODE_PREFIX_0xec_SET_5_H;
      break;
    }
    case 0xed: {
      opcode->prefix = OPCODE_PREFIX_0xed_SET_5_L;
      break;
    }
    case 0xee: {
      opcode->prefix = OPCODE_PREFIX_0xee_SET_5_pHL;
      break;
    }
    case 0xef: {
      opcode->prefix = OPCODE_PREFIX_0xef_SET_5_A;
      break;
    }
    case 0xf0: {
      opcode->prefix = OPCODE_PREFIX_0xf0_SET_6_B;
      break;
    }
    case 0xf1: {
      opcode->prefix = OPCODE_PREFIX_0xf1_SET_6_C;
      break;
    }
    case 0xf2: {
      opcode->prefix = OPCODE_PREFIX_0xf2_SET_6_D;
      break;
    }
    case 0xf3: {
      opcode->prefix = OPCODE_PREFIX_0xf3_SET_6_E;
      break;
    }
    case 0xf4: {
      opcode->prefix = OPCODE_PREFIX_0xf4_SET_6_H;
      break;
    }
    case 0xf5: {
      opcode->prefix = OPCODE_PREFIX_0xf5_SET_6_L;
      break;
    }
    case 0xf6: {
      opcode->prefix = OPCODE_PREFIX_0xf6_SET_6_pHL;
      break;
    }
    case 0xf7: {
      opcode->prefix = OPCODE_PREFIX_0xf7_SET_6_A;
      break;
    }
    case 0xf8: {
      opcode->prefix = OPCODE_PREFIX_0xf8_SET_7_B;
      break;
    }
    case 0xf9: {
      opcode->prefix = OPCODE_PREFIX_0xf9_SET_7_C;
      break;
    }
    case 0xfa: {
      opcode->prefix = OPCODE_PREFIX_0xfa_SET_7_D;
      break;
    }
    case 0xfb: {
      opcode->prefix = OPCODE_PREFIX_0xfb_SET_7_E;
      break;
    }
    case 0xfc: {
      opcode->prefix = OPCODE_PREFIX_0xfc_SET_7_H;
      break;
    }
    case 0xfd: {
      opcode->prefix = OPCODE_PREFIX_0xfd_SET_7_L;
      break;
    }
    case 0xfe: {
      opcode->prefix = OPCODE_PREFIX_0xfe_SET_7_pHL;
      break;
    }
    case 0xff: {
      opcode->prefix = OPCODE_PREFIX_0xff_SET_7_A;
      break;
    }
    default:
      __builtin_unreachable();
    }
    break;
  }
  default: {
    FATAL("Cannot decode opcode 0x(%02x)\n", kind);
  }
  }
}

uint8_t execute(Emu *emu, const Opcode *const opcode) { // NOLINT
  switch (opcode->kind) {
  case OPCODE_KIND_0x00_NOP: {
    return 4;
  }
  case OPCODE_KIND_0x06_LD_B_n8: {
    emu->cpu.reg.b = opcode->n8;
    return 8;
  }
  case OPCODE_KIND_0x0e_LD_C_n8: {
    emu->cpu.reg.c = opcode->n8;
    return 8;
  }
  case OPCODE_KIND_0x16_LD_D_n8: {
    emu->cpu.reg.d = opcode->n8;
    return 8;
  }
  case OPCODE_KIND_0x1e_LD_E_n8: {
    emu->cpu.reg.e = opcode->n8;
    return 8;
  }
  case OPCODE_KIND_0x26_LD_H_n8: {
    emu->cpu.reg.h = opcode->n8;
    return 8;
  }
  case OPCODE_KIND_0x2e_LD_L_n8: {
    emu->cpu.reg.l = opcode->n8;
    return 8;
  }
  case OPCODE_KIND_0x36_LD_pHL_n8: {
    mem_write_u8(&emu->mem, reg_hl(&emu->cpu.reg), opcode->n8);
    return 12;
  }
  case OPCODE_KIND_0x3e_LD_A_n8: {
    emu->cpu.reg.a = opcode->n8;
    return 8;
  }
  case OPCODE_KIND_0xc3_JP_a16: {
    emu->cpu.reg.pc = opcode->a16;
    return 16;
  }
  case OPCODE_KIND_0x01_LD_BC_n16: {
    reg_set_bc(&emu->cpu.reg, opcode->n16);
    return 12;
  }
  case OPCODE_KIND_0x11_LD_DE_n16: {
    reg_set_de(&emu->cpu.reg, opcode->n16);
    return 12;
  }
  case OPCODE_KIND_0x21_LD_HL_n16: {
    reg_set_hl(&emu->cpu.reg, opcode->n16);
    return 12;
  }
  case OPCODE_KIND_0x31_LD_SP_n16: {
    emu->cpu.reg.sp = opcode->n16;
    return 12;
  }
  case OPCODE_KIND_0x40_LD_B_B: {
    emu->cpu.reg.b = emu->cpu.reg.b;
    return 4;
  }
  case OPCODE_KIND_0x41_LD_B_C: {
    emu->cpu.reg.b = emu->cpu.reg.c;
    return 4;
  }
  case OPCODE_KIND_0x42_LD_B_D: {
    emu->cpu.reg.b = emu->cpu.reg.d;
    return 4;
  }
  case OPCODE_KIND_0x43_LD_B_E: {
    emu->cpu.reg.b = emu->cpu.reg.e;
    return 4;
  }
  case OPCODE_KIND_0x44_LD_B_H: {
    emu->cpu.reg.b = emu->cpu.reg.h;
    return 4;
  }
  case OPCODE_KIND_0x45_LD_B_L: {
    emu->cpu.reg.b = emu->cpu.reg.l;
    return 4;
  }
  case OPCODE_KIND_0x46_LD_B_pHL: {
    emu->cpu.reg.b = mem_read_u8(&emu->mem, reg_hl(&emu->cpu.reg));
    return 8;
  }
  case OPCODE_KIND_0x47_LD_B_A: {
    emu->cpu.reg.b = emu->cpu.reg.a;
    return 4;
  }
  case OPCODE_KIND_0x48_LD_C_B: {
    emu->cpu.reg.c = emu->cpu.reg.b;
    return 4;
  }
  case OPCODE_KIND_0x49_LD_C_C: {
    emu->cpu.reg.c = emu->cpu.reg.c;
    return 4;
  }
  case OPCODE_KIND_0x4a_LD_C_D: {
    emu->cpu.reg.c = emu->cpu.reg.d;
    return 4;
  }
  case OPCODE_KIND_0x4b_LD_C_E: {
    emu->cpu.reg.c = emu->cpu.reg.e;
    return 4;
  }
  case OPCODE_KIND_0x4c_LD_C_H: {
    emu->cpu.reg.c = emu->cpu.reg.h;
    return 4;
  }
  case OPCODE_KIND_0x4d_LD_C_L: {
    emu->cpu.reg.c = emu->cpu.reg.l;
    return 4;
  }
  case OPCODE_KIND_0x4e_LD_C_pHL: {
    emu->cpu.reg.c = mem_read_u8(&emu->mem, reg_hl(&emu->cpu.reg));
    return 8;
  }
  case OPCODE_KIND_0x4f_LD_C_A: {
    emu->cpu.reg.c = emu->cpu.reg.a;
    return 4;
  }
  case OPCODE_KIND_0x50_LD_D_B: {
    emu->cpu.reg.d = emu->cpu.reg.b;
    return 4;
  }
  case OPCODE_KIND_0x51_LD_D_C: {
    emu->cpu.reg.d = emu->cpu.reg.c;
    return 4;
  }
  case OPCODE_KIND_0x52_LD_D_D: {
    emu->cpu.reg.d = emu->cpu.reg.d;
    return 4;
  }
  case OPCODE_KIND_0x53_LD_D_E: {
    emu->cpu.reg.d = emu->cpu.reg.e;
    return 4;
  }
  case OPCODE_KIND_0x54_LD_D_H: {
    emu->cpu.reg.d = emu->cpu.reg.h;
    return 4;
  }
  case OPCODE_KIND_0x55_LD_D_L: {
    emu->cpu.reg.d = emu->cpu.reg.l;
    return 4;
  }
  case OPCODE_KIND_0x56_LD_D_pHL: {
    emu->cpu.reg.d = mem_read_u8(&emu->mem, reg_hl(&emu->cpu.reg));
    return 8;
  }
  case OPCODE_KIND_0x57_LD_D_A: {
    emu->cpu.reg.d = emu->cpu.reg.a;
    return 4;
  }
  case OPCODE_KIND_0x58_LD_E_B: {
    emu->cpu.reg.e = emu->cpu.reg.b;
    return 4;
  }
  case OPCODE_KIND_0x59_LD_E_C: {
    emu->cpu.reg.e = emu->cpu.reg.c;
    return 4;
  }
  case OPCODE_KIND_0x5a_LD_E_D: {
    emu->cpu.reg.e = emu->cpu.reg.d;
    return 4;
  }
  case OPCODE_KIND_0x5b_LD_E_E: {
    emu->cpu.reg.e = emu->cpu.reg.e;
    return 4;
  }
  case OPCODE_KIND_0x5c_LD_E_H: {
    emu->cpu.reg.e = emu->cpu.reg.h;
    return 4;
  }
  case OPCODE_KIND_0x5d_LD_E_L: {
    emu->cpu.reg.e = emu->cpu.reg.l;
    return 4;
  }
  case OPCODE_KIND_0x5e_LD_E_pHL: {
    emu->cpu.reg.e = mem_read_u8(&emu->mem, reg_hl(&emu->cpu.reg));
    return 8;
  }
  case OPCODE_KIND_0x5f_LD_E_A: {
    emu->cpu.reg.e = emu->cpu.reg.a;
    return 4;
  }
  case OPCODE_KIND_0x60_LD_H_B: {
    emu->cpu.reg.h = emu->cpu.reg.b;
    return 4;
  }
  case OPCODE_KIND_0x61_LD_H_C: {
    emu->cpu.reg.h = emu->cpu.reg.c;
    return 4;
  }
  case OPCODE_KIND_0x62_LD_H_D: {
    emu->cpu.reg.h = emu->cpu.reg.d;
    return 4;
  }
  case OPCODE_KIND_0x63_LD_H_E: {
    emu->cpu.reg.h = emu->cpu.reg.e;
    return 4;
  }
  case OPCODE_KIND_0x64_LD_H_H: {
    emu->cpu.reg.h = emu->cpu.reg.h;
    return 4;
  }
  case OPCODE_KIND_0x65_LD_H_L: {
    emu->cpu.reg.h = emu->cpu.reg.l;
    return 4;
  }
  case OPCODE_KIND_0x66_LD_H_pHL: {
    emu->cpu.reg.h = mem_read_u8(&emu->mem, reg_hl(&emu->cpu.reg));
    return 8;
  }
  case OPCODE_KIND_0x67_LD_H_A: {
    emu->cpu.reg.h = emu->cpu.reg.a;
    return 4;
  }
  case OPCODE_KIND_0x68_LD_L_B: {
    emu->cpu.reg.l = emu->cpu.reg.b;
    return 4;
  }
  case OPCODE_KIND_0x69_LD_L_C: {
    emu->cpu.reg.l = emu->cpu.reg.c;
    return 4;
  }
  case OPCODE_KIND_0x6a_LD_L_D: {
    emu->cpu.reg.l = emu->cpu.reg.d;
    return 4;
  }
  case OPCODE_KIND_0x6b_LD_L_E: {
    emu->cpu.reg.l = emu->cpu.reg.e;
    return 4;
  }
  case OPCODE_KIND_0x6c_LD_L_H: {
    emu->cpu.reg.l = emu->cpu.reg.h;
    return 4;
  }
  case OPCODE_KIND_0x6d_LD_L_L: {
    emu->cpu.reg.l = emu->cpu.reg.l;
    return 4;
  }
  case OPCODE_KIND_0x6e_LD_L_pHL: {
    emu->cpu.reg.l = mem_read_u8(&emu->mem, reg_hl(&emu->cpu.reg));
    return 8;
  }
  case OPCODE_KIND_0x6f_LD_L_A: {
    emu->cpu.reg.l = emu->cpu.reg.a;
    return 4;
  }
  case OPCODE_KIND_0x70_LD_pHL_B: {
    mem_write_u8(&emu->mem, reg_hl(&emu->cpu.reg), emu->cpu.reg.b);
    return 8;
  }
  case OPCODE_KIND_0x71_LD_pHL_C: {
    mem_write_u8(&emu->mem, reg_hl(&emu->cpu.reg), emu->cpu.reg.c);
    return 8;
  }
  case OPCODE_KIND_0x72_LD_pHL_D: {
    mem_write_u8(&emu->mem, reg_hl(&emu->cpu.reg), emu->cpu.reg.d);
    return 8;
  }
  case OPCODE_KIND_0x73_LD_pHL_E: {
    mem_write_u8(&emu->mem, reg_hl(&emu->cpu.reg), emu->cpu.reg.e);
    return 8;
  }
  case OPCODE_KIND_0x74_LD_pHL_H: {
    mem_write_u8(&emu->mem, reg_hl(&emu->cpu.reg), emu->cpu.reg.h);
    return 8;
  }
  case OPCODE_KIND_0x75_LD_pHL_L: {
    mem_write_u8(&emu->mem, reg_hl(&emu->cpu.reg), emu->cpu.reg.l);
    return 8;
  }
  case OPCODE_KIND_0x77_LD_pHL_A: {
    mem_write_u8(&emu->mem, reg_hl(&emu->cpu.reg), emu->cpu.reg.a);
    return 8;
  }
  case OPCODE_KIND_0x78_LD_A_B: {
    emu->cpu.reg.a = emu->cpu.reg.b;
    return 4;
  }
  case OPCODE_KIND_0x79_LD_A_C: {
    emu->cpu.reg.a = emu->cpu.reg.c;
    return 4;
  }
  case OPCODE_KIND_0x7a_LD_A_D: {
    emu->cpu.reg.a = emu->cpu.reg.d;
    return 4;
  }
  case OPCODE_KIND_0x7b_LD_A_E: {
    emu->cpu.reg.a = emu->cpu.reg.e;
    return 4;
  }
  case OPCODE_KIND_0x7c_LD_A_H: {
    emu->cpu.reg.a = emu->cpu.reg.h;
    return 4;
  }
  case OPCODE_KIND_0x7d_LD_A_L: {
    emu->cpu.reg.a = emu->cpu.reg.l;
    return 4;
  }
  case OPCODE_KIND_0x7e_LD_A_pHL: {
    emu->cpu.reg.a = mem_read_u8(&emu->mem, reg_hl(&emu->cpu.reg));
    return 4;
  }
  case OPCODE_KIND_0x7f_LD_A_A: {
    emu->cpu.reg.a = emu->cpu.reg.a;
    return 4;
  }
  case OPCODE_KIND_0x0a_LD_A_pBC: {
    emu->cpu.reg.a = mem_read_u8(&emu->mem, reg_bc(&emu->cpu.reg));
    return 8;
  }
  case OPCODE_KIND_0x1a_LD_A_pDE: {
    emu->cpu.reg.a = mem_read_u8(&emu->mem, reg_de(&emu->cpu.reg));
    return 8;
  }
  case OPCODE_KIND_0x2a_LD_A_pHLi: {
    {
      uint16_t hl = reg_hl(&emu->cpu.reg);
      emu->cpu.reg.a = mem_read_u8(&emu->mem, hl);
      reg_set_hl(&emu->cpu.reg, hl + 1);
      return 8;
    }
  }
  case OPCODE_KIND_0x3a_LD_A_pHLd: {
    uint16_t hl = reg_hl(&emu->cpu.reg);
    emu->cpu.reg.a = mem_read_u8(&emu->mem, hl);
    reg_set_hl(&emu->cpu.reg, hl - 1);
    return 8;
  }
  case OPCODE_KIND_0x02_LD_pBC_A: {
    mem_write_u8(&emu->mem, reg_bc(&emu->cpu.reg), emu->cpu.reg.a);
    return 8;
  }
  case OPCODE_KIND_0x12_LD_pDE_A: {
    mem_write_u8(&emu->mem, reg_de(&emu->cpu.reg), emu->cpu.reg.a);
    return 8;
  }
  case OPCODE_KIND_0x22_LD_pHLi_A: {
    uint16_t hl = reg_hl(&emu->cpu.reg);
    mem_write_u8(&emu->mem, hl, emu->cpu.reg.a);
    reg_set_hl(&emu->cpu.reg, hl + 1);
    return 8;
  }
  case OPCODE_KIND_0x32_LD_pHLd_A: {
    uint16_t hl = reg_hl(&emu->cpu.reg);
    mem_write_u8(&emu->mem, hl, emu->cpu.reg.a);
    reg_set_hl(&emu->cpu.reg, hl - 1);
    return 8;
  }
  case OPCODE_KIND_0x04_INC_B: {
    cpu_inc_r8(&emu->cpu, &emu->cpu.reg.b);
    return 4;
  }
  case OPCODE_KIND_0x0c_INC_C: {
    cpu_inc_r8(&emu->cpu, &emu->cpu.reg.c);
    return 4;
  }
  case OPCODE_KIND_0x14_INC_D: {
    cpu_inc_r8(&emu->cpu, &emu->cpu.reg.d);
    return 4;
  }
  case OPCODE_KIND_0x1c_INC_E: {
    cpu_inc_r8(&emu->cpu, &emu->cpu.reg.e);
    return 4;
  }
  case OPCODE_KIND_0x24_INC_H: {
    cpu_inc_r8(&emu->cpu, &emu->cpu.reg.h);
    return 4;
  }
  case OPCODE_KIND_0x2c_INC_L: {
    cpu_inc_r8(&emu->cpu, &emu->cpu.reg.l);
    return 4;
  }
  case OPCODE_KIND_0x34_INC_pHL: {
    uint16_t hl = reg_hl(&emu->cpu.reg);
    uint8_t byte = mem_read_u8(&emu->mem, hl);
    cpu_inc_r8(&emu->cpu, &byte);
    mem_write_u8(&emu->mem, hl, byte);
    return 12;
  }
  case OPCODE_KIND_0x3c_INC_A: {
    cpu_inc_r8(&emu->cpu, &emu->cpu.reg.a);
    return 4;
  }
  case OPCODE_KIND_0x05_DEC_B: {
    cpu_dec_r8(&emu->cpu, &emu->cpu.reg.b);
    return 4;
  }
  case OPCODE_KIND_0x0d_DEC_C: {
    cpu_dec_r8(&emu->cpu, &emu->cpu.reg.c);
    return 4;
  }
  case OPCODE_KIND_0x15_DEC_D: {
    cpu_dec_r8(&emu->cpu, &emu->cpu.reg.d);
    return 4;
  }
  case OPCODE_KIND_0x1d_DEC_E: {
    cpu_dec_r8(&emu->cpu, &emu->cpu.reg.e);
    return 4;
  }
  case OPCODE_KIND_0x25_DEC_H: {
    cpu_dec_r8(&emu->cpu, &emu->cpu.reg.h);
    return 4;
  }
  case OPCODE_KIND_0x2d_DEC_L: {
    cpu_dec_r8(&emu->cpu, &emu->cpu.reg.l);
    return 4;
  }
  case OPCODE_KIND_0x35_DEC_pHL: {
    uint16_t hl = reg_hl(&emu->cpu.reg);
    uint8_t byte = mem_read_u8(&emu->mem, hl);
    cpu_dec_r8(&emu->cpu, &byte);
    mem_write_u8(&emu->mem, hl, byte);
    return 12;
  }
  case OPCODE_KIND_0x3d_DEC_A: {
    cpu_dec_r8(&emu->cpu, &emu->cpu.reg.a);
    return 4;
  }
  case OPCODE_KIND_0x18_JR_e8: {
    emu->cpu.reg.pc += opcode->e8;
    return 12;
  }
  case OPCODE_KIND_0x28_JR_Z_e8: {
    if (emu->cpu.reg.flags.z) {
      emu->cpu.reg.pc += opcode->e8;
      return 12;
    }
    return 8;
  }
  case OPCODE_KIND_0x38_JR_C_e8: {
    if (emu->cpu.reg.flags.c) {
      emu->cpu.reg.pc += opcode->e8;
      return 12;
    }
    return 8;
  }
  case OPCODE_KIND_0x20_JR_NZ_e8: {
    if (!emu->cpu.reg.flags.z) {
      emu->cpu.reg.pc += opcode->e8;
      return 12;
    }
    return 8;
  }
  case OPCODE_KIND_0x30_JR_NC_e8: {
    if (!emu->cpu.reg.flags.c) {
      emu->cpu.reg.pc += opcode->e8;
      return 12;
    }
    return 8;
  }
  case OPCODE_KIND_0xf3_DI: {
    emu->cpu.ime = false;
    return 4;
  }
  case OPCODE_KIND_0xfb_EI: {
    emu->cpu.ei_waiting = true;
    return 4;
  }
  case OPCODE_KIND_0xea_LD_pa16_A: {
    mem_write_u8(&emu->mem, opcode->a16, emu->cpu.reg.a);
    return 16;
  }
  case OPCODE_KIND_0xfa_LD_A_pa16: {
    emu->cpu.reg.a = mem_read_u8(&emu->mem, opcode->a16);
    return 16;
  }
  case OPCODE_KIND_0xe0_LDH_pa8_A: {
    mem_write_u8(&emu->mem, 0xFF00 | opcode->a8, emu->cpu.reg.a);
    return 12;
  }
  case OPCODE_KIND_0xf0_LDH_A_pa8: {
    emu->cpu.reg.a = mem_read_u8(&emu->mem, 0xFF00 | opcode->a8);
    return 12;
  }
  case OPCODE_KIND_0xe2_LDH_pC_A: {
    mem_write_u8(&emu->mem, 0xFF00 | emu->cpu.reg.c, emu->cpu.reg.a);
    return 8;
  }
  case OPCODE_KIND_0xf2_LDH_A_pC: {
    emu->cpu.reg.a = mem_read_u8(&emu->mem, 0xFF00 | emu->cpu.reg.c);
    return 8;
  }
  case OPCODE_KIND_0xcd_CALL_a16: {
    cpu_call(&emu->cpu, &emu->mem, opcode->a16);
    return 24;
  }
  case OPCODE_KIND_0xcc_CALL_Z_a16: {
    if (emu->cpu.reg.flags.z) {
      cpu_call(&emu->cpu, &emu->mem, opcode->a16);
      return 24;
    }
    return 12;
  }
  case OPCODE_KIND_0xdc_CALL_C_a16: {
    if (emu->cpu.reg.flags.c) {
      cpu_call(&emu->cpu, &emu->mem, opcode->a16);
      return 24;
    }
    return 12;
  }
  case OPCODE_KIND_0xc4_CALL_NZ_a16: {
    if (!emu->cpu.reg.flags.z) {
      cpu_call(&emu->cpu, &emu->mem, opcode->a16);
      return 24;
    }
    return 12;
  }
  case OPCODE_KIND_0xd4_CALL_NC_a16: {
    if (!emu->cpu.reg.flags.c) {
      cpu_call(&emu->cpu, &emu->mem, opcode->a16);
      return 24;
    }
    return 12;
  }
  case OPCODE_KIND_0xc9_RET: {
    cpu_ret(&emu->cpu, &emu->mem);
    return 16;
  }
  case OPCODE_KIND_0xc8_RET_Z: {
    if (emu->cpu.reg.flags.z) {
      cpu_ret(&emu->cpu, &emu->mem);
      return 20;
    }
    return 8;
  }
  case OPCODE_KIND_0xd8_RET_C: {
    if (emu->cpu.reg.flags.c) {
      cpu_ret(&emu->cpu, &emu->mem);
      return 20;
    }
    return 8;
  }
  case OPCODE_KIND_0xc0_RET_NZ: {
    if (!emu->cpu.reg.flags.z) {
      cpu_ret(&emu->cpu, &emu->mem);
      return 20;
    }
    return 8;
  }
  case OPCODE_KIND_0xd0_RET_NC: {
    if (!emu->cpu.reg.flags.c) {
      cpu_ret(&emu->cpu, &emu->mem);
      return 20;
    }
    return 8;
  }
  case OPCODE_KIND_0xc5_PUSH_BC: {
    cpu_push(&emu->cpu, &emu->mem, reg_bc(&emu->cpu.reg));
    return 16;
  }
  case OPCODE_KIND_0xd5_PUSH_DE: {
    cpu_push(&emu->cpu, &emu->mem, reg_de(&emu->cpu.reg));
    return 16;
  }
  case OPCODE_KIND_0xe5_PUSH_HL: {
    cpu_push(&emu->cpu, &emu->mem, reg_hl(&emu->cpu.reg));
    return 16;
  }
  case OPCODE_KIND_0xf5_PUSH_AF: {
    cpu_push(&emu->cpu, &emu->mem, reg_af(&emu->cpu.reg));
    return 16;
  }
  case OPCODE_KIND_0xc1_POP_BC: {
    uint16_t r = cpu_pop(&emu->cpu, &emu->mem);
    reg_set_bc(&emu->cpu.reg, r);
    return 12;
  }
  case OPCODE_KIND_0xd1_POP_DE: {
    uint16_t r = cpu_pop(&emu->cpu, &emu->mem);
    reg_set_de(&emu->cpu.reg, r);
    return 12;
  }
  case OPCODE_KIND_0xe1_POP_HL: {
    uint16_t r = cpu_pop(&emu->cpu, &emu->mem);
    reg_set_hl(&emu->cpu.reg, r);
    return 12;
  }
  case OPCODE_KIND_0xf1_POP_AF: {
    uint16_t r = cpu_pop(&emu->cpu, &emu->mem);
    reg_set_af(&emu->cpu.reg, r);
    return 12;
  }
  case OPCODE_KIND_0x03_INC_BC: {
    reg_set_bc(&emu->cpu.reg, reg_bc(&emu->cpu.reg) + 1);
    return 8;
  }
  case OPCODE_KIND_0x13_INC_DE: {
    reg_set_de(&emu->cpu.reg, reg_de(&emu->cpu.reg) + 1);
    return 8;
  }
  case OPCODE_KIND_0x23_INC_HL: {
    reg_set_hl(&emu->cpu.reg, reg_hl(&emu->cpu.reg) + 1);
    return 8;
  }
  case OPCODE_KIND_0x33_INC_SP: {
    emu->cpu.reg.sp++;
    return 8;
  }
  case OPCODE_KIND_0x0b_DEC_BC: {
    reg_set_bc(&emu->cpu.reg, reg_bc(&emu->cpu.reg) - 1);
    return 8;
  }
  case OPCODE_KIND_0x1b_DEC_DE: {
    reg_set_de(&emu->cpu.reg, reg_de(&emu->cpu.reg) - 1);
    return 8;
  }
  case OPCODE_KIND_0x2b_DEC_HL: {
    reg_set_hl(&emu->cpu.reg, reg_hl(&emu->cpu.reg) - 1);
    return 8;
  }
  case OPCODE_KIND_0x3b_DEC_SP: {
    emu->cpu.reg.sp--;
    return 8;
  }
  case OPCODE_KIND_0x80_ADD_A_B: {
    cpu_add(&emu->cpu, emu->cpu.reg.b);
    return 4;
  }
  case OPCODE_KIND_0x81_ADD_A_C: {
    cpu_add(&emu->cpu, emu->cpu.reg.c);
    return 4;
  }
  case OPCODE_KIND_0x82_ADD_A_D: {
    cpu_add(&emu->cpu, emu->cpu.reg.d);
    return 4;
  }
  case OPCODE_KIND_0x83_ADD_A_E: {
    cpu_add(&emu->cpu, emu->cpu.reg.e);
    return 4;
  }
  case OPCODE_KIND_0x84_ADD_A_H: {
    cpu_add(&emu->cpu, emu->cpu.reg.h);
    return 4;
  }
  case OPCODE_KIND_0x85_ADD_A_L: {
    cpu_add(&emu->cpu, emu->cpu.reg.l);
    return 4;
  }
  case OPCODE_KIND_0x86_ADD_A_pHL: {
    cpu_add(&emu->cpu, mem_read_u8(&emu->mem, reg_hl(&emu->cpu.reg)));
    return 4;
  }
  case OPCODE_KIND_0x87_ADD_A_A: {
    cpu_add(&emu->cpu, emu->cpu.reg.a);
    return 4;
  }
  case OPCODE_KIND_0x88_ADC_A_B: {
    cpu_adc(&emu->cpu, emu->cpu.reg.b);
    return 4;
  }
  case OPCODE_KIND_0x89_ADC_A_C: {
    cpu_adc(&emu->cpu, emu->cpu.reg.c);
    return 4;
  }
  case OPCODE_KIND_0x8a_ADC_A_D: {
    cpu_adc(&emu->cpu, emu->cpu.reg.d);
    return 4;
  }
  case OPCODE_KIND_0x8b_ADC_A_E: {
    cpu_adc(&emu->cpu, emu->cpu.reg.e);
    return 4;
  }
  case OPCODE_KIND_0x8c_ADC_A_H: {
    cpu_adc(&emu->cpu, emu->cpu.reg.h);
    return 4;
  }
  case OPCODE_KIND_0x8d_ADC_A_L: {
    cpu_adc(&emu->cpu, emu->cpu.reg.l);
    return 4;
  }
  case OPCODE_KIND_0x8e_ADC_A_pHL: {
    cpu_adc(&emu->cpu, mem_read_u8(&emu->mem, reg_hl(&emu->cpu.reg)));
    return 4;
  }
  case OPCODE_KIND_0x8f_ADC_A_A: {
    cpu_adc(&emu->cpu, emu->cpu.reg.a);
    return 4;
  }
  case OPCODE_KIND_0x90_SUB_A_B: {
    cpu_sub(&emu->cpu, emu->cpu.reg.b);
    return 4;
  }
  case OPCODE_KIND_0x91_SUB_A_C: {
    cpu_sub(&emu->cpu, emu->cpu.reg.c);
    return 4;
  }
  case OPCODE_KIND_0x92_SUB_A_D: {
    cpu_sub(&emu->cpu, emu->cpu.reg.d);
    return 4;
  }
  case OPCODE_KIND_0x93_SUB_A_E: {
    cpu_sub(&emu->cpu, emu->cpu.reg.e);
    return 4;
  }
  case OPCODE_KIND_0x94_SUB_A_H: {
    cpu_sub(&emu->cpu, emu->cpu.reg.h);
    return 4;
  }
  case OPCODE_KIND_0x95_SUB_A_L: {
    cpu_sub(&emu->cpu, emu->cpu.reg.l);
    return 4;
  }
  case OPCODE_KIND_0x96_SUB_A_pHL: {
    cpu_sub(&emu->cpu, mem_read_u8(&emu->mem, reg_hl(&emu->cpu.reg)));
    return 4;
  }
  case OPCODE_KIND_0x97_SUB_A_A: {
    cpu_sub(&emu->cpu, emu->cpu.reg.a);
    return 4;
  }
  case OPCODE_KIND_0x98_SBC_A_B: {
    cpu_sbc(&emu->cpu, emu->cpu.reg.b);
    return 4;
  }
  case OPCODE_KIND_0x99_SBC_A_C: {
    cpu_sbc(&emu->cpu, emu->cpu.reg.c);
    return 4;
  }
  case OPCODE_KIND_0x9a_SBC_A_D: {
    cpu_sbc(&emu->cpu, emu->cpu.reg.d);
    return 4;
  }
  case OPCODE_KIND_0x9b_SBC_A_E: {
    cpu_sbc(&emu->cpu, emu->cpu.reg.e);
    return 4;
  }
  case OPCODE_KIND_0x9c_SBC_A_H: {
    cpu_sbc(&emu->cpu, emu->cpu.reg.h);
    return 4;
  }
  case OPCODE_KIND_0x9d_SBC_A_L: {
    cpu_sbc(&emu->cpu, emu->cpu.reg.l);
    return 4;
  }
  case OPCODE_KIND_0x9e_SBC_A_pHL: {
    cpu_sbc(&emu->cpu, mem_read_u8(&emu->mem, reg_hl(&emu->cpu.reg)));
    return 4;
  }
  case OPCODE_KIND_0x9f_SBC_A_A: {
    cpu_sbc(&emu->cpu, emu->cpu.reg.a);
    return 4;
  }
  case OPCODE_KIND_0xa0_AND_A_B: {
    cpu_and(&emu->cpu, emu->cpu.reg.b);
    return 4;
  }
  case OPCODE_KIND_0xa1_AND_A_C: {
    cpu_and(&emu->cpu, emu->cpu.reg.c);
    return 4;
  }
  case OPCODE_KIND_0xa2_AND_A_D: {
    cpu_and(&emu->cpu, emu->cpu.reg.d);
    return 4;
  }
  case OPCODE_KIND_0xa3_AND_A_E: {
    cpu_and(&emu->cpu, emu->cpu.reg.e);
    return 4;
  }
  case OPCODE_KIND_0xa4_AND_A_H: {
    cpu_and(&emu->cpu, emu->cpu.reg.h);
    return 4;
  }
  case OPCODE_KIND_0xa5_AND_A_L: {
    cpu_and(&emu->cpu, emu->cpu.reg.l);
    return 4;
  }
  case OPCODE_KIND_0xa6_AND_A_pHL: {
    cpu_and(&emu->cpu, mem_read_u8(&emu->mem, reg_hl(&emu->cpu.reg)));
    return 4;
  }
  case OPCODE_KIND_0xa7_AND_A_A: {
    cpu_and(&emu->cpu, emu->cpu.reg.a);
    return 4;
  }
  case OPCODE_KIND_0xa8_XOR_A_B: {
    cpu_xor(&emu->cpu, emu->cpu.reg.b);
    return 4;
  }
  case OPCODE_KIND_0xa9_XOR_A_C: {
    cpu_xor(&emu->cpu, emu->cpu.reg.c);
    return 4;
  }
  case OPCODE_KIND_0xaa_XOR_A_D: {
    cpu_xor(&emu->cpu, emu->cpu.reg.d);
    return 4;
  }
  case OPCODE_KIND_0xab_XOR_A_E: {
    cpu_xor(&emu->cpu, emu->cpu.reg.e);
    return 4;
  }
  case OPCODE_KIND_0xac_XOR_A_H: {
    cpu_xor(&emu->cpu, emu->cpu.reg.h);
    return 4;
  }
  case OPCODE_KIND_0xad_XOR_A_L: {
    cpu_xor(&emu->cpu, emu->cpu.reg.l);
    return 4;
  }
  case OPCODE_KIND_0xae_XOR_A_pHL: {
    cpu_xor(&emu->cpu, mem_read_u8(&emu->mem, reg_hl(&emu->cpu.reg)));
    return 4;
  }
  case OPCODE_KIND_0xaf_XOR_A_A: {
    cpu_xor(&emu->cpu, emu->cpu.reg.a);
    return 4;
  }
  case OPCODE_KIND_0xb0_OR_A_B: {
    cpu_or(&emu->cpu, emu->cpu.reg.b);
    return 4;
  }
  case OPCODE_KIND_0xb1_OR_A_C: {
    cpu_or(&emu->cpu, emu->cpu.reg.c);
    return 4;
  }
  case OPCODE_KIND_0xb2_OR_A_D: {
    cpu_or(&emu->cpu, emu->cpu.reg.d);
    return 4;
  }
  case OPCODE_KIND_0xb3_OR_A_E: {
    cpu_or(&emu->cpu, emu->cpu.reg.e);
    return 4;
  }
  case OPCODE_KIND_0xb4_OR_A_H: {
    cpu_or(&emu->cpu, emu->cpu.reg.h);
    return 4;
  }
  case OPCODE_KIND_0xb5_OR_A_L: {
    cpu_or(&emu->cpu, emu->cpu.reg.l);
    return 4;
  }
  case OPCODE_KIND_0xb6_OR_A_pHL: {
    cpu_or(&emu->cpu, mem_read_u8(&emu->mem, reg_hl(&emu->cpu.reg)));
    return 4;
  }
  case OPCODE_KIND_0xb7_OR_A_A: {
    cpu_or(&emu->cpu, emu->cpu.reg.a);
    return 4;
  }
  case OPCODE_KIND_0xb8_CP_A_B: {
    cpu_cp(&emu->cpu, emu->cpu.reg.b);
    return 4;
  }
  case OPCODE_KIND_0xb9_CP_A_C: {
    cpu_cp(&emu->cpu, emu->cpu.reg.c);
    return 4;
  }
  case OPCODE_KIND_0xba_CP_A_D: {
    cpu_cp(&emu->cpu, emu->cpu.reg.d);
    return 4;
  }
  case OPCODE_KIND_0xbb_CP_A_E: {
    cpu_cp(&emu->cpu, emu->cpu.reg.e);
    return 4;
  }
  case OPCODE_KIND_0xbc_CP_A_H: {
    cpu_cp(&emu->cpu, emu->cpu.reg.h);
    return 4;
  }
  case OPCODE_KIND_0xbd_CP_A_L: {
    cpu_cp(&emu->cpu, emu->cpu.reg.l);
    return 4;
  }
  case OPCODE_KIND_0xbe_CP_A_pHL: {
    cpu_cp(&emu->cpu, mem_read_u8(&emu->mem, reg_hl(&emu->cpu.reg)));
    return 4;
  }
  case OPCODE_KIND_0xbf_CP_A_A: {
    cpu_cp(&emu->cpu, emu->cpu.reg.a);
    return 4;
  }
  case OPCODE_KIND_0xc6_ADD_A_n8: {
    cpu_add(&emu->cpu, opcode->n8);
    return 8;
  }
  case OPCODE_KIND_0xce_ADC_A_n8: {
    cpu_adc(&emu->cpu, opcode->n8);
    return 8;
  }
  case OPCODE_KIND_0xd6_SUB_A_n8: {
    cpu_sub(&emu->cpu, opcode->n8);
    return 8;
  }
  case OPCODE_KIND_0xde_SBC_A_n8: {
    cpu_sbc(&emu->cpu, opcode->n8);
    return 8;
  }
  case OPCODE_KIND_0xe6_AND_A_n8: {
    cpu_and(&emu->cpu, opcode->n8);
    return 8;
  }
  case OPCODE_KIND_0xee_XOR_A_n8: {
    cpu_xor(&emu->cpu, opcode->n8);
    return 8;
  }
  case OPCODE_KIND_0xf6_OR_A_n8: {
    cpu_or(&emu->cpu, opcode->n8);
    return 8;
  }
  case OPCODE_KIND_0xfe_CP_A_n8: {
    cpu_cp(&emu->cpu, opcode->n8);
    return 8;
  }
  case OPCODE_KIND_0x07_RLCA: {
    cpu_rlca(&emu->cpu);
    return 4;
  }
  case OPCODE_KIND_0x17_RLA: {
    cpu_rla(&emu->cpu);
    return 4;
  }
  case OPCODE_KIND_0x0f_RRCA: {
    cpu_rrca(&emu->cpu);
    return 4;
  }
  case OPCODE_KIND_0x1f_RRA: {
    cpu_rra(&emu->cpu);
    return 4;
  }
  case OPCODE_KIND_0x09_ADD_HL_BC: {
    cpu_add_hl(&emu->cpu, reg_bc(&emu->cpu.reg));
    return 8;
  }
  case OPCODE_KIND_0x19_ADD_HL_DE: {
    cpu_add_hl(&emu->cpu, reg_de(&emu->cpu.reg));
    return 8;
  }
  case OPCODE_KIND_0x29_ADD_HL_HL: {
    cpu_add_hl(&emu->cpu, reg_hl(&emu->cpu.reg));
    return 8;
  }
  case OPCODE_KIND_0x39_ADD_HL_SP: {
    cpu_add_hl(&emu->cpu, emu->cpu.reg.sp);
    return 8;
  }
  case OPCODE_KIND_0xe9_JP_HL: {
    emu->cpu.reg.pc = reg_hl(&emu->cpu.reg);
    return 4;
  }
  case OPCODE_KIND_0xca_JP_Z_a16: {
    if (emu->cpu.reg.flags.z) {
      emu->cpu.reg.pc = opcode->a16;
      return 16;
    }
    return 12;
  }
  case OPCODE_KIND_0xda_JP_C_a16: {
    if (emu->cpu.reg.flags.c) {
      emu->cpu.reg.pc = opcode->a16;
      return 16;
    }
    return 12;
  }
  case OPCODE_KIND_0xc2_JP_NZ_a16: {
    if (!emu->cpu.reg.flags.z) {
      emu->cpu.reg.pc = opcode->a16;
      return 16;
    }
    return 12;
  }
  case OPCODE_KIND_0xd2_JP_NC_a16: {
    if (!emu->cpu.reg.flags.c) {
      emu->cpu.reg.pc = opcode->a16;
      return 16;
    }
    return 12;
  }
  case OPCODE_KIND_0x27_DAA: {
    cpu_daa(&emu->cpu);
    return 4;
  }
  case OPCODE_KIND_0x08_LD_pa16_SP: {
    mem_write_u16(&emu->mem, opcode->a16, emu->cpu.reg.sp);
    return 20;
  }
  case OPCODE_KIND_0xf9_LD_SP_HL: {
    emu->cpu.reg.sp = reg_hl(&emu->cpu.reg);
    return 8;
  }
  case OPCODE_KIND_0xe8_ADD_SP_e8: {
    cpu_add_sp_e8(&emu->cpu, opcode->e8);
    return 16;
  }
  case OPCODE_KIND_0xf8_LD_HL_SP_e8: {
    cpu_ld_hl_sp_e8(&emu->cpu, opcode->e8);
    return 12;
  }
  case OPCODE_KIND_0xd9_RETI: {
    cpu_ret(&emu->cpu, &emu->mem);
    emu->cpu.ime = true;
    return 16;
  }
  case OPCODE_KIND_0xc7_RST_0x00: {
    cpu_call(&emu->cpu, &emu->mem, 0x00);
    return 16;
  }
  case OPCODE_KIND_0xd7_RST_0x10: {
    cpu_call(&emu->cpu, &emu->mem, 0x10);
    return 16;
  }
  case OPCODE_KIND_0xe7_RST_0x20: {
    cpu_call(&emu->cpu, &emu->mem, 0x20);
    return 16;
  }
  case OPCODE_KIND_0xf7_RST_0x30: {
    cpu_call(&emu->cpu, &emu->mem, 0x30);
    return 16;
  }
  case OPCODE_KIND_0xcf_RST_0x08: {
    cpu_call(&emu->cpu, &emu->mem, 0x08);
    return 16;
  }
  case OPCODE_KIND_0xdf_RST_0x18: {
    cpu_call(&emu->cpu, &emu->mem, 0x18);
    return 16;
  }
  case OPCODE_KIND_0xef_RST_0x28: {
    cpu_call(&emu->cpu, &emu->mem, 0x28);
    return 16;
  }
  case OPCODE_KIND_0xff_RST_0x38: {
    cpu_call(&emu->cpu, &emu->mem, 0x38);
    return 16;
  }
  case OPCODE_KIND_0x2f_CPL: {
    cpu_cpl(&emu->cpu);
    return 4;
  }
  case OPCODE_KIND_0x3f_CCF: {
    cpu_ccf(&emu->cpu);
    return 4;
  }
  case OPCODE_KIND_0x37_SCF: {
    cpu_scf(&emu->cpu);
    return 4;
  }
  case OPCODE_KIND_0x76_HALT: {
    emu->cpu.halted = true;
    emu->cpu.halt_bug =
        (bool)(!emu->cpu.ime && (int)cpu_has_interrupt_pending(&emu->mem));
    return 4;
  }
  case OPCODE_KIND_0xcb_PREFIX: {
    switch (opcode->prefix) {
    case OPCODE_PREFIX_0x00_RLC_B: {
      cpu_rlc(&emu->cpu, &emu->cpu.reg.b);
      return 8;
    }
    case OPCODE_PREFIX_0x01_RLC_C: {
      cpu_rlc(&emu->cpu, &emu->cpu.reg.c);
      return 8;
    }
    case OPCODE_PREFIX_0x02_RLC_D: {
      cpu_rlc(&emu->cpu, &emu->cpu.reg.d);
      return 8;
    }
    case OPCODE_PREFIX_0x03_RLC_E: {
      cpu_rlc(&emu->cpu, &emu->cpu.reg.e);
      return 8;
    }
    case OPCODE_PREFIX_0x04_RLC_H: {
      cpu_rlc(&emu->cpu, &emu->cpu.reg.h);
      return 8;
    }
    case OPCODE_PREFIX_0x05_RLC_L: {
      cpu_rlc(&emu->cpu, &emu->cpu.reg.l);
      return 8;
    }
    case OPCODE_PREFIX_0x06_RLC_pHL: {
      cpu_rlc(&emu->cpu, mem_ref_u8(&emu->mem, reg_hl(&emu->cpu.reg)));
      return 16;
    }
    case OPCODE_PREFIX_0x07_RLC_A: {
      cpu_rlc(&emu->cpu, &emu->cpu.reg.a);
      return 8;
    }
    case OPCODE_PREFIX_0x08_RRC_B: {
      cpu_rrc(&emu->cpu, &emu->cpu.reg.b);
      return 8;
    }
    case OPCODE_PREFIX_0x09_RRC_C: {
      cpu_rrc(&emu->cpu, &emu->cpu.reg.c);
      return 8;
    }
    case OPCODE_PREFIX_0x0a_RRC_D: {
      cpu_rrc(&emu->cpu, &emu->cpu.reg.d);
      return 8;
    }
    case OPCODE_PREFIX_0x0b_RRC_E: {
      cpu_rrc(&emu->cpu, &emu->cpu.reg.e);
      return 8;
    }
    case OPCODE_PREFIX_0x0c_RRC_H: {
      cpu_rrc(&emu->cpu, &emu->cpu.reg.h);
      return 8;
    }
    case OPCODE_PREFIX_0x0d_RRC_L: {
      cpu_rrc(&emu->cpu, &emu->cpu.reg.l);
      return 8;
    }
    case OPCODE_PREFIX_0x0e_RRC_pHL: {
      cpu_rrc(&emu->cpu, mem_ref_u8(&emu->mem, reg_hl(&emu->cpu.reg)));
      return 16;
    }
    case OPCODE_PREFIX_0x0f_RRC_A: {
      cpu_rrc(&emu->cpu, &emu->cpu.reg.a);
      return 8;
    }
    case OPCODE_PREFIX_0x10_RL_B: {
      cpu_rl(&emu->cpu, &emu->cpu.reg.b);
      return 8;
    }
    case OPCODE_PREFIX_0x11_RL_C: {
      cpu_rl(&emu->cpu, &emu->cpu.reg.c);
      return 8;
    }
    case OPCODE_PREFIX_0x12_RL_D: {
      cpu_rl(&emu->cpu, &emu->cpu.reg.d);
      return 8;
    }
    case OPCODE_PREFIX_0x13_RL_E: {
      cpu_rl(&emu->cpu, &emu->cpu.reg.e);
      return 8;
    }
    case OPCODE_PREFIX_0x14_RL_H: {
      cpu_rl(&emu->cpu, &emu->cpu.reg.h);
      return 8;
    }
    case OPCODE_PREFIX_0x15_RL_L: {
      cpu_rl(&emu->cpu, &emu->cpu.reg.l);
      return 8;
    }
    case OPCODE_PREFIX_0x16_RL_pHL: {
      cpu_rl(&emu->cpu, mem_ref_u8(&emu->mem, reg_hl(&emu->cpu.reg)));
      return 16;
    }
    case OPCODE_PREFIX_0x17_RL_A: {
      cpu_rl(&emu->cpu, &emu->cpu.reg.a);
      return 8;
    }
    case OPCODE_PREFIX_0x18_RR_B: {
      cpu_rr(&emu->cpu, &emu->cpu.reg.b);
      return 8;
    }
    case OPCODE_PREFIX_0x19_RR_C: {
      cpu_rr(&emu->cpu, &emu->cpu.reg.c);
      return 8;
    }
    case OPCODE_PREFIX_0x1a_RR_D: {
      cpu_rr(&emu->cpu, &emu->cpu.reg.d);
      return 8;
    }
    case OPCODE_PREFIX_0x1b_RR_E: {
      cpu_rr(&emu->cpu, &emu->cpu.reg.e);
      return 8;
    }
    case OPCODE_PREFIX_0x1c_RR_H: {
      cpu_rr(&emu->cpu, &emu->cpu.reg.h);
      return 8;
    }
    case OPCODE_PREFIX_0x1d_RR_L: {
      cpu_rr(&emu->cpu, &emu->cpu.reg.l);
      return 8;
    }
    case OPCODE_PREFIX_0x1e_RR_pHL: {
      cpu_rr(&emu->cpu, mem_ref_u8(&emu->mem, reg_hl(&emu->cpu.reg)));
      return 16;
    }
    case OPCODE_PREFIX_0x1f_RR_A: {
      cpu_rr(&emu->cpu, &emu->cpu.reg.a);
      return 8;
    }
    case OPCODE_PREFIX_0x20_SLA_B: {
      cpu_sla(&emu->cpu, &emu->cpu.reg.b);
      return 8;
    }
    case OPCODE_PREFIX_0x21_SLA_C: {
      cpu_sla(&emu->cpu, &emu->cpu.reg.c);
      return 8;
    }
    case OPCODE_PREFIX_0x22_SLA_D: {
      cpu_sla(&emu->cpu, &emu->cpu.reg.d);
      return 8;
    }
    case OPCODE_PREFIX_0x23_SLA_E: {
      cpu_sla(&emu->cpu, &emu->cpu.reg.e);
      return 8;
    }
    case OPCODE_PREFIX_0x24_SLA_H: {
      cpu_sla(&emu->cpu, &emu->cpu.reg.h);
      return 8;
    }
    case OPCODE_PREFIX_0x25_SLA_L: {
      cpu_sla(&emu->cpu, &emu->cpu.reg.l);
      return 8;
    }
    case OPCODE_PREFIX_0x26_SLA_pHL: {
      cpu_sla(&emu->cpu, mem_ref_u8(&emu->mem, reg_hl(&emu->cpu.reg)));
      return 16;
    }
    case OPCODE_PREFIX_0x27_SLA_A: {
      cpu_sla(&emu->cpu, &emu->cpu.reg.a);
      return 8;
    }
    case OPCODE_PREFIX_0x28_SRA_B: {
      cpu_sra(&emu->cpu, &emu->cpu.reg.b);
      return 8;
    }
    case OPCODE_PREFIX_0x29_SRA_C: {
      cpu_sra(&emu->cpu, &emu->cpu.reg.c);
      return 8;
    }
    case OPCODE_PREFIX_0x2a_SRA_D: {
      cpu_sra(&emu->cpu, &emu->cpu.reg.d);
      return 8;
    }
    case OPCODE_PREFIX_0x2b_SRA_E: {
      cpu_sra(&emu->cpu, &emu->cpu.reg.e);
      return 8;
    }
    case OPCODE_PREFIX_0x2c_SRA_H: {
      cpu_sra(&emu->cpu, &emu->cpu.reg.h);
      return 8;
    }
    case OPCODE_PREFIX_0x2d_SRA_L: {
      cpu_sra(&emu->cpu, &emu->cpu.reg.l);
      return 8;
    }
    case OPCODE_PREFIX_0x2e_SRA_pHL: {
      cpu_sra(&emu->cpu, mem_ref_u8(&emu->mem, reg_hl(&emu->cpu.reg)));
      return 16;
    }
    case OPCODE_PREFIX_0x2f_SRA_A: {
      cpu_sra(&emu->cpu, &emu->cpu.reg.a);
      return 8;
    }
    case OPCODE_PREFIX_0x30_SWAP_B: {
      cpu_swap(&emu->cpu, &emu->cpu.reg.b);
      return 8;
    }
    case OPCODE_PREFIX_0x31_SWAP_C: {
      cpu_swap(&emu->cpu, &emu->cpu.reg.c);
      return 8;
    }
    case OPCODE_PREFIX_0x32_SWAP_D: {
      cpu_swap(&emu->cpu, &emu->cpu.reg.d);
      return 8;
    }
    case OPCODE_PREFIX_0x33_SWAP_E: {
      cpu_swap(&emu->cpu, &emu->cpu.reg.e);
      return 8;
    }
    case OPCODE_PREFIX_0x34_SWAP_H: {
      cpu_swap(&emu->cpu, &emu->cpu.reg.h);
      return 8;
    }
    case OPCODE_PREFIX_0x35_SWAP_L: {
      cpu_swap(&emu->cpu, &emu->cpu.reg.l);
      return 8;
    }
    case OPCODE_PREFIX_0x36_SWAP_pHL: {
      cpu_swap(&emu->cpu, mem_ref_u8(&emu->mem, reg_hl(&emu->cpu.reg)));
      return 16;
    }
    case OPCODE_PREFIX_0x37_SWAP_A: {
      cpu_swap(&emu->cpu, &emu->cpu.reg.a);
      return 8;
    }
    case OPCODE_PREFIX_0x38_SRL_B: {
      cpu_srl(&emu->cpu, &emu->cpu.reg.b);
      return 8;
    }
    case OPCODE_PREFIX_0x39_SRL_C: {
      cpu_srl(&emu->cpu, &emu->cpu.reg.c);
      return 8;
    }
    case OPCODE_PREFIX_0x3a_SRL_D: {
      cpu_srl(&emu->cpu, &emu->cpu.reg.d);
      return 8;
    }
    case OPCODE_PREFIX_0x3b_SRL_E: {
      cpu_srl(&emu->cpu, &emu->cpu.reg.e);
      return 8;
    }
    case OPCODE_PREFIX_0x3c_SRL_H: {
      cpu_srl(&emu->cpu, &emu->cpu.reg.h);
      return 8;
    }
    case OPCODE_PREFIX_0x3d_SRL_L: {
      cpu_srl(&emu->cpu, &emu->cpu.reg.l);
      return 8;
    }
    case OPCODE_PREFIX_0x3e_SRL_pHL: {
      cpu_srl(&emu->cpu, mem_ref_u8(&emu->mem, reg_hl(&emu->cpu.reg)));
      return 16;
    }
    case OPCODE_PREFIX_0x3f_SRL_A: {
      cpu_srl(&emu->cpu, &emu->cpu.reg.a);
      return 8;
    }
    case OPCODE_PREFIX_0x40_BIT_0_B: {
      cpu_bit(0, &emu->cpu, emu->cpu.reg.b);
      return 8;
    }
    case OPCODE_PREFIX_0x41_BIT_0_C: {
      cpu_bit(0, &emu->cpu, emu->cpu.reg.c);
      return 8;
    }
    case OPCODE_PREFIX_0x42_BIT_0_D: {
      cpu_bit(0, &emu->cpu, emu->cpu.reg.d);
      return 8;
    }
    case OPCODE_PREFIX_0x43_BIT_0_E: {
      cpu_bit(0, &emu->cpu, emu->cpu.reg.e);
      return 8;
    }
    case OPCODE_PREFIX_0x44_BIT_0_H: {
      cpu_bit(0, &emu->cpu, emu->cpu.reg.h);
      return 8;
    }
    case OPCODE_PREFIX_0x45_BIT_0_L: {
      cpu_bit(0, &emu->cpu, emu->cpu.reg.l);
      return 8;
    }
    case OPCODE_PREFIX_0x46_BIT_0_pHL: {
      cpu_bit(0, &emu->cpu, mem_read_u8(&emu->mem, reg_hl(&emu->cpu.reg)));
      return 12;
    }
    case OPCODE_PREFIX_0x47_BIT_0_A: {
      cpu_bit(0, &emu->cpu, emu->cpu.reg.a);
      return 8;
    }
    case OPCODE_PREFIX_0x48_BIT_1_B: {
      cpu_bit(1, &emu->cpu, emu->cpu.reg.b);
      return 8;
    }
    case OPCODE_PREFIX_0x49_BIT_1_C: {
      cpu_bit(1, &emu->cpu, emu->cpu.reg.c);
      return 8;
    }
    case OPCODE_PREFIX_0x4a_BIT_1_D: {
      cpu_bit(1, &emu->cpu, emu->cpu.reg.d);
      return 8;
    }
    case OPCODE_PREFIX_0x4b_BIT_1_E: {
      cpu_bit(1, &emu->cpu, emu->cpu.reg.e);
      return 8;
    }
    case OPCODE_PREFIX_0x4c_BIT_1_H: {
      cpu_bit(1, &emu->cpu, emu->cpu.reg.h);
      return 8;
    }
    case OPCODE_PREFIX_0x4d_BIT_1_L: {
      cpu_bit(1, &emu->cpu, emu->cpu.reg.l);
      return 8;
    }
    case OPCODE_PREFIX_0x4e_BIT_1_pHL: {
      cpu_bit(1, &emu->cpu, mem_read_u8(&emu->mem, reg_hl(&emu->cpu.reg)));
      return 12;
    }
    case OPCODE_PREFIX_0x4f_BIT_1_A: {
      cpu_bit(1, &emu->cpu, emu->cpu.reg.a);
      return 8;
    }
    case OPCODE_PREFIX_0x50_BIT_2_B: {
      cpu_bit(2, &emu->cpu, emu->cpu.reg.b);
      return 8;
    }
    case OPCODE_PREFIX_0x51_BIT_2_C: {
      cpu_bit(2, &emu->cpu, emu->cpu.reg.c);
      return 8;
    }
    case OPCODE_PREFIX_0x52_BIT_2_D: {
      cpu_bit(2, &emu->cpu, emu->cpu.reg.d);
      return 8;
    }
    case OPCODE_PREFIX_0x53_BIT_2_E: {
      cpu_bit(2, &emu->cpu, emu->cpu.reg.e);
      return 8;
    }
    case OPCODE_PREFIX_0x54_BIT_2_H: {
      cpu_bit(2, &emu->cpu, emu->cpu.reg.h);
      return 8;
    }
    case OPCODE_PREFIX_0x55_BIT_2_L: {
      cpu_bit(2, &emu->cpu, emu->cpu.reg.l);
      return 8;
    }
    case OPCODE_PREFIX_0x56_BIT_2_pHL: {
      cpu_bit(2, &emu->cpu, mem_read_u8(&emu->mem, reg_hl(&emu->cpu.reg)));
      return 12;
    }
    case OPCODE_PREFIX_0x57_BIT_2_A: {
      cpu_bit(2, &emu->cpu, emu->cpu.reg.a);
      return 8;
    }
    case OPCODE_PREFIX_0x58_BIT_3_B: {
      cpu_bit(3, &emu->cpu, emu->cpu.reg.b);
      return 8;
    }
    case OPCODE_PREFIX_0x59_BIT_3_C: {
      cpu_bit(3, &emu->cpu, emu->cpu.reg.c);
      return 8;
    }
    case OPCODE_PREFIX_0x5a_BIT_3_D: {
      cpu_bit(3, &emu->cpu, emu->cpu.reg.d);
      return 8;
    }
    case OPCODE_PREFIX_0x5b_BIT_3_E: {
      cpu_bit(3, &emu->cpu, emu->cpu.reg.e);
      return 8;
    }
    case OPCODE_PREFIX_0x5c_BIT_3_H: {
      cpu_bit(3, &emu->cpu, emu->cpu.reg.h);
      return 8;
    }
    case OPCODE_PREFIX_0x5d_BIT_3_L: {
      cpu_bit(3, &emu->cpu, emu->cpu.reg.l);
      return 8;
    }
    case OPCODE_PREFIX_0x5e_BIT_3_pHL: {
      cpu_bit(3, &emu->cpu, mem_read_u8(&emu->mem, reg_hl(&emu->cpu.reg)));
      return 12;
    }
    case OPCODE_PREFIX_0x5f_BIT_3_A: {
      cpu_bit(3, &emu->cpu, emu->cpu.reg.a);
      return 8;
    }
    case OPCODE_PREFIX_0x60_BIT_4_B: {
      cpu_bit(4, &emu->cpu, emu->cpu.reg.b);
      return 8;
    }
    case OPCODE_PREFIX_0x61_BIT_4_C: {
      cpu_bit(4, &emu->cpu, emu->cpu.reg.c);
      return 8;
    }
    case OPCODE_PREFIX_0x62_BIT_4_D: {
      cpu_bit(4, &emu->cpu, emu->cpu.reg.d);
      return 8;
    }
    case OPCODE_PREFIX_0x63_BIT_4_E: {
      cpu_bit(4, &emu->cpu, emu->cpu.reg.e);
      return 8;
    }
    case OPCODE_PREFIX_0x64_BIT_4_H: {
      cpu_bit(4, &emu->cpu, emu->cpu.reg.h);
      return 8;
    }
    case OPCODE_PREFIX_0x65_BIT_4_L: {
      cpu_bit(4, &emu->cpu, emu->cpu.reg.l);
      return 8;
    }
    case OPCODE_PREFIX_0x66_BIT_4_pHL: {
      cpu_bit(4, &emu->cpu, mem_read_u8(&emu->mem, reg_hl(&emu->cpu.reg)));
      return 12;
    }
    case OPCODE_PREFIX_0x67_BIT_4_A: {
      cpu_bit(4, &emu->cpu, emu->cpu.reg.a);
      return 8;
    }
    case OPCODE_PREFIX_0x68_BIT_5_B: {
      cpu_bit(5, &emu->cpu, emu->cpu.reg.b);
      return 8;
    }
    case OPCODE_PREFIX_0x69_BIT_5_C: {
      cpu_bit(5, &emu->cpu, emu->cpu.reg.c);
      return 8;
    }
    case OPCODE_PREFIX_0x6a_BIT_5_D: {
      cpu_bit(5, &emu->cpu, emu->cpu.reg.d);
      return 8;
    }
    case OPCODE_PREFIX_0x6b_BIT_5_E: {
      cpu_bit(5, &emu->cpu, emu->cpu.reg.e);
      return 8;
    }
    case OPCODE_PREFIX_0x6c_BIT_5_H: {
      cpu_bit(5, &emu->cpu, emu->cpu.reg.h);
      return 8;
    }
    case OPCODE_PREFIX_0x6d_BIT_5_L: {
      cpu_bit(5, &emu->cpu, emu->cpu.reg.l);
      return 8;
    }
    case OPCODE_PREFIX_0x6e_BIT_5_pHL: {
      cpu_bit(5, &emu->cpu, mem_read_u8(&emu->mem, reg_hl(&emu->cpu.reg)));
      return 12;
    }
    case OPCODE_PREFIX_0x6f_BIT_5_A: {
      cpu_bit(5, &emu->cpu, emu->cpu.reg.a);
      return 8;
    }
    case OPCODE_PREFIX_0x70_BIT_6_B: {
      cpu_bit(6, &emu->cpu, emu->cpu.reg.b);
      return 8;
    }
    case OPCODE_PREFIX_0x71_BIT_6_C: {
      cpu_bit(6, &emu->cpu, emu->cpu.reg.c);
      return 8;
    }
    case OPCODE_PREFIX_0x72_BIT_6_D: {
      cpu_bit(6, &emu->cpu, emu->cpu.reg.d);
      return 8;
    }
    case OPCODE_PREFIX_0x73_BIT_6_E: {
      cpu_bit(6, &emu->cpu, emu->cpu.reg.e);
      return 8;
    }
    case OPCODE_PREFIX_0x74_BIT_6_H: {
      cpu_bit(6, &emu->cpu, emu->cpu.reg.h);
      return 8;
    }
    case OPCODE_PREFIX_0x75_BIT_6_L: {
      cpu_bit(6, &emu->cpu, emu->cpu.reg.l);
      return 8;
    }
    case OPCODE_PREFIX_0x76_BIT_6_pHL: {
      cpu_bit(6, &emu->cpu, mem_read_u8(&emu->mem, reg_hl(&emu->cpu.reg)));
      return 12;
    }
    case OPCODE_PREFIX_0x77_BIT_6_A: {
      cpu_bit(6, &emu->cpu, emu->cpu.reg.a);
      return 8;
    }
    case OPCODE_PREFIX_0x78_BIT_7_B: {
      cpu_bit(7, &emu->cpu, emu->cpu.reg.b);
      return 8;
    }
    case OPCODE_PREFIX_0x79_BIT_7_C: {
      cpu_bit(7, &emu->cpu, emu->cpu.reg.c);
      return 8;
    }
    case OPCODE_PREFIX_0x7a_BIT_7_D: {
      cpu_bit(7, &emu->cpu, emu->cpu.reg.d);
      return 8;
    }
    case OPCODE_PREFIX_0x7b_BIT_7_E: {
      cpu_bit(7, &emu->cpu, emu->cpu.reg.e);
      return 8;
    }
    case OPCODE_PREFIX_0x7c_BIT_7_H: {
      cpu_bit(7, &emu->cpu, emu->cpu.reg.h);
      return 8;
    }
    case OPCODE_PREFIX_0x7d_BIT_7_L: {
      cpu_bit(7, &emu->cpu, emu->cpu.reg.l);
      return 8;
    }
    case OPCODE_PREFIX_0x7e_BIT_7_pHL: {
      cpu_bit(7, &emu->cpu, mem_read_u8(&emu->mem, reg_hl(&emu->cpu.reg)));
      return 12;
    }
    case OPCODE_PREFIX_0x7f_BIT_7_A: {
      cpu_bit(7, &emu->cpu, emu->cpu.reg.a);
      return 8;
    }
    case OPCODE_PREFIX_0x80_RES_0_B: {
      cpu_res(0, &emu->cpu.reg.b);
      return 8;
    }
    case OPCODE_PREFIX_0x81_RES_0_C: {
      cpu_res(0, &emu->cpu.reg.c);
      return 8;
    }
    case OPCODE_PREFIX_0x82_RES_0_D: {
      cpu_res(0, &emu->cpu.reg.d);
      return 8;
    }
    case OPCODE_PREFIX_0x83_RES_0_E: {
      cpu_res(0, &emu->cpu.reg.e);
      return 8;
    }
    case OPCODE_PREFIX_0x84_RES_0_H: {
      cpu_res(0, &emu->cpu.reg.h);
      return 8;
    }
    case OPCODE_PREFIX_0x85_RES_0_L: {
      cpu_res(0, &emu->cpu.reg.l);
      return 8;
    }
    case OPCODE_PREFIX_0x86_RES_0_pHL: {
      cpu_res(0, mem_ref_u8(&emu->mem, reg_hl(&emu->cpu.reg)));
      return 16;
    }
    case OPCODE_PREFIX_0x87_RES_0_A: {
      cpu_res(0, &emu->cpu.reg.a);
      return 8;
    }
    case OPCODE_PREFIX_0x88_RES_1_B: {
      cpu_res(1, &emu->cpu.reg.b);
      return 8;
    }
    case OPCODE_PREFIX_0x89_RES_1_C: {
      cpu_res(1, &emu->cpu.reg.c);
      return 8;
    }
    case OPCODE_PREFIX_0x8a_RES_1_D: {
      cpu_res(1, &emu->cpu.reg.d);
      return 8;
    }
    case OPCODE_PREFIX_0x8b_RES_1_E: {
      cpu_res(1, &emu->cpu.reg.e);
      return 8;
    }
    case OPCODE_PREFIX_0x8c_RES_1_H: {
      cpu_res(1, &emu->cpu.reg.h);
      return 8;
    }
    case OPCODE_PREFIX_0x8d_RES_1_L: {
      cpu_res(1, &emu->cpu.reg.l);
      return 8;
    }
    case OPCODE_PREFIX_0x8e_RES_1_pHL: {
      cpu_res(1, mem_ref_u8(&emu->mem, reg_hl(&emu->cpu.reg)));
      return 16;
    }
    case OPCODE_PREFIX_0x8f_RES_1_A: {
      cpu_res(1, &emu->cpu.reg.a);
      return 8;
    }
    case OPCODE_PREFIX_0x90_RES_2_B: {
      cpu_res(2, &emu->cpu.reg.b);
      return 8;
    }
    case OPCODE_PREFIX_0x91_RES_2_C: {
      cpu_res(2, &emu->cpu.reg.c);
      return 8;
    }
    case OPCODE_PREFIX_0x92_RES_2_D: {
      cpu_res(2, &emu->cpu.reg.d);
      return 8;
    }
    case OPCODE_PREFIX_0x93_RES_2_E: {
      cpu_res(2, &emu->cpu.reg.e);
      return 8;
    }
    case OPCODE_PREFIX_0x94_RES_2_H: {
      cpu_res(2, &emu->cpu.reg.h);
      return 8;
    }
    case OPCODE_PREFIX_0x95_RES_2_L: {
      cpu_res(2, &emu->cpu.reg.l);
      return 8;
    }
    case OPCODE_PREFIX_0x96_RES_2_pHL: {
      cpu_res(2, mem_ref_u8(&emu->mem, reg_hl(&emu->cpu.reg)));
      return 16;
    }
    case OPCODE_PREFIX_0x97_RES_2_A: {
      cpu_res(2, &emu->cpu.reg.a);
      return 8;
    }
    case OPCODE_PREFIX_0x98_RES_3_B: {
      cpu_res(3, &emu->cpu.reg.b);
      return 8;
    }
    case OPCODE_PREFIX_0x99_RES_3_C: {
      cpu_res(3, &emu->cpu.reg.c);
      return 8;
    }
    case OPCODE_PREFIX_0x9a_RES_3_D: {
      cpu_res(3, &emu->cpu.reg.d);
      return 8;
    }
    case OPCODE_PREFIX_0x9b_RES_3_E: {
      cpu_res(3, &emu->cpu.reg.e);
      return 8;
    }
    case OPCODE_PREFIX_0x9c_RES_3_H: {
      cpu_res(3, &emu->cpu.reg.h);
      return 8;
    }
    case OPCODE_PREFIX_0x9d_RES_3_L: {
      cpu_res(3, &emu->cpu.reg.l);
      return 8;
    }
    case OPCODE_PREFIX_0x9e_RES_3_pHL: {
      cpu_res(3, mem_ref_u8(&emu->mem, reg_hl(&emu->cpu.reg)));
      return 16;
    }
    case OPCODE_PREFIX_0x9f_RES_3_A: {
      cpu_res(3, &emu->cpu.reg.a);
      return 8;
    }
    case OPCODE_PREFIX_0xa0_RES_4_B: {
      cpu_res(4, &emu->cpu.reg.b);
      return 8;
    }
    case OPCODE_PREFIX_0xa1_RES_4_C: {
      cpu_res(4, &emu->cpu.reg.c);
      return 8;
    }
    case OPCODE_PREFIX_0xa2_RES_4_D: {
      cpu_res(4, &emu->cpu.reg.d);
      return 8;
    }
    case OPCODE_PREFIX_0xa3_RES_4_E: {
      cpu_res(4, &emu->cpu.reg.e);
      return 8;
    }
    case OPCODE_PREFIX_0xa4_RES_4_H: {
      cpu_res(4, &emu->cpu.reg.h);
      return 8;
    }
    case OPCODE_PREFIX_0xa5_RES_4_L: {
      cpu_res(4, &emu->cpu.reg.l);
      return 8;
    }
    case OPCODE_PREFIX_0xa6_RES_4_pHL: {
      cpu_res(4, mem_ref_u8(&emu->mem, reg_hl(&emu->cpu.reg)));
      return 16;
    }
    case OPCODE_PREFIX_0xa7_RES_4_A: {
      cpu_res(4, &emu->cpu.reg.a);
      return 8;
    }
    case OPCODE_PREFIX_0xa8_RES_5_B: {
      cpu_res(5, &emu->cpu.reg.b);
      return 8;
    }
    case OPCODE_PREFIX_0xa9_RES_5_C: {
      cpu_res(5, &emu->cpu.reg.c);
      return 8;
    }
    case OPCODE_PREFIX_0xaa_RES_5_D: {
      cpu_res(5, &emu->cpu.reg.d);
      return 8;
    }
    case OPCODE_PREFIX_0xab_RES_5_E: {
      cpu_res(5, &emu->cpu.reg.e);
      return 8;
    }
    case OPCODE_PREFIX_0xac_RES_5_H: {
      cpu_res(5, &emu->cpu.reg.h);
      return 8;
    }
    case OPCODE_PREFIX_0xad_RES_5_L: {
      cpu_res(5, &emu->cpu.reg.l);
      return 8;
    }
    case OPCODE_PREFIX_0xae_RES_5_pHL: {
      cpu_res(5, mem_ref_u8(&emu->mem, reg_hl(&emu->cpu.reg)));
      return 16;
    }
    case OPCODE_PREFIX_0xaf_RES_5_A: {
      cpu_res(5, &emu->cpu.reg.a);
      return 8;
    }
    case OPCODE_PREFIX_0xb0_RES_6_B: {
      cpu_res(6, &emu->cpu.reg.b);
      return 8;
    }
    case OPCODE_PREFIX_0xb1_RES_6_C: {
      cpu_res(6, &emu->cpu.reg.c);
      return 8;
    }
    case OPCODE_PREFIX_0xb2_RES_6_D: {
      cpu_res(6, &emu->cpu.reg.d);
      return 8;
    }
    case OPCODE_PREFIX_0xb3_RES_6_E: {
      cpu_res(6, &emu->cpu.reg.e);
      return 8;
    }
    case OPCODE_PREFIX_0xb4_RES_6_H: {
      cpu_res(6, &emu->cpu.reg.h);
      return 8;
    }
    case OPCODE_PREFIX_0xb5_RES_6_L: {
      cpu_res(6, &emu->cpu.reg.l);
      return 8;
    }
    case OPCODE_PREFIX_0xb6_RES_6_pHL: {
      cpu_res(6, mem_ref_u8(&emu->mem, reg_hl(&emu->cpu.reg)));
      return 16;
    }
    case OPCODE_PREFIX_0xb7_RES_6_A: {
      cpu_res(6, &emu->cpu.reg.a);
      return 8;
    }
    case OPCODE_PREFIX_0xb8_RES_7_B: {
      cpu_res(7, &emu->cpu.reg.b);
      return 8;
    }
    case OPCODE_PREFIX_0xb9_RES_7_C: {
      cpu_res(7, &emu->cpu.reg.c);
      return 8;
    }
    case OPCODE_PREFIX_0xba_RES_7_D: {
      cpu_res(7, &emu->cpu.reg.d);
      return 8;
    }
    case OPCODE_PREFIX_0xbb_RES_7_E: {
      cpu_res(7, &emu->cpu.reg.e);
      return 8;
    }
    case OPCODE_PREFIX_0xbc_RES_7_H: {
      cpu_res(7, &emu->cpu.reg.h);
      return 8;
    }
    case OPCODE_PREFIX_0xbd_RES_7_L: {
      cpu_res(7, &emu->cpu.reg.l);
      return 8;
    }
    case OPCODE_PREFIX_0xbe_RES_7_pHL: {
      cpu_res(7, mem_ref_u8(&emu->mem, reg_hl(&emu->cpu.reg)));
      return 16;
    }
    case OPCODE_PREFIX_0xbf_RES_7_A: {
      cpu_res(7, &emu->cpu.reg.a);
      return 8;
    }
    case OPCODE_PREFIX_0xc0_SET_0_B: {
      cpu_set(0, &emu->cpu.reg.b);
      return 8;
    }
    case OPCODE_PREFIX_0xc1_SET_0_C: {
      cpu_set(0, &emu->cpu.reg.c);
      return 8;
    }
    case OPCODE_PREFIX_0xc2_SET_0_D: {
      cpu_set(0, &emu->cpu.reg.d);
      return 8;
    }
    case OPCODE_PREFIX_0xc3_SET_0_E: {
      cpu_set(0, &emu->cpu.reg.e);
      return 8;
    }
    case OPCODE_PREFIX_0xc4_SET_0_H: {
      cpu_set(0, &emu->cpu.reg.h);
      return 8;
    }
    case OPCODE_PREFIX_0xc5_SET_0_L: {
      cpu_set(0, &emu->cpu.reg.l);
      return 8;
    }
    case OPCODE_PREFIX_0xc6_SET_0_pHL: {
      cpu_set(0, mem_ref_u8(&emu->mem, reg_hl(&emu->cpu.reg)));
      return 16;
    }
    case OPCODE_PREFIX_0xc7_SET_0_A: {
      cpu_set(0, &emu->cpu.reg.a);
      return 8;
    }
    case OPCODE_PREFIX_0xc8_SET_1_B: {
      cpu_set(1, &emu->cpu.reg.b);
      return 8;
    }
    case OPCODE_PREFIX_0xc9_SET_1_C: {
      cpu_set(1, &emu->cpu.reg.c);
      return 8;
    }
    case OPCODE_PREFIX_0xca_SET_1_D: {
      cpu_set(1, &emu->cpu.reg.d);
      return 8;
    }
    case OPCODE_PREFIX_0xcb_SET_1_E: {
      cpu_set(1, &emu->cpu.reg.e);
      return 8;
    }
    case OPCODE_PREFIX_0xcc_SET_1_H: {
      cpu_set(1, &emu->cpu.reg.h);
      return 8;
    }
    case OPCODE_PREFIX_0xcd_SET_1_L: {
      cpu_set(1, &emu->cpu.reg.l);
      return 8;
    }
    case OPCODE_PREFIX_0xce_SET_1_pHL: {
      cpu_set(1, mem_ref_u8(&emu->mem, reg_hl(&emu->cpu.reg)));
      return 16;
    }
    case OPCODE_PREFIX_0xcf_SET_1_A: {
      cpu_set(1, &emu->cpu.reg.a);
      return 8;
    }
    case OPCODE_PREFIX_0xd0_SET_2_B: {
      cpu_set(2, &emu->cpu.reg.b);
      return 8;
    }
    case OPCODE_PREFIX_0xd1_SET_2_C: {
      cpu_set(2, &emu->cpu.reg.c);
      return 8;
    }
    case OPCODE_PREFIX_0xd2_SET_2_D: {
      cpu_set(2, &emu->cpu.reg.d);
      return 8;
    }
    case OPCODE_PREFIX_0xd3_SET_2_E: {
      cpu_set(2, &emu->cpu.reg.e);
      return 8;
    }
    case OPCODE_PREFIX_0xd4_SET_2_H: {
      cpu_set(2, &emu->cpu.reg.h);
      return 8;
    }
    case OPCODE_PREFIX_0xd5_SET_2_L: {
      cpu_set(2, &emu->cpu.reg.l);
      return 8;
    }
    case OPCODE_PREFIX_0xd6_SET_2_pHL: {
      cpu_set(2, mem_ref_u8(&emu->mem, reg_hl(&emu->cpu.reg)));
      return 16;
    }
    case OPCODE_PREFIX_0xd7_SET_2_A: {
      cpu_set(2, &emu->cpu.reg.a);
      return 8;
    }
    case OPCODE_PREFIX_0xd8_SET_3_B: {
      cpu_set(3, &emu->cpu.reg.b);
      return 8;
    }
    case OPCODE_PREFIX_0xd9_SET_3_C: {
      cpu_set(3, &emu->cpu.reg.c);
      return 8;
    }
    case OPCODE_PREFIX_0xda_SET_3_D: {
      cpu_set(3, &emu->cpu.reg.d);
      return 8;
    }
    case OPCODE_PREFIX_0xdb_SET_3_E: {
      cpu_set(3, &emu->cpu.reg.e);
      return 8;
    }
    case OPCODE_PREFIX_0xdc_SET_3_H: {
      cpu_set(3, &emu->cpu.reg.h);
      return 8;
    }
    case OPCODE_PREFIX_0xdd_SET_3_L: {
      cpu_set(3, &emu->cpu.reg.l);
      return 8;
    }
    case OPCODE_PREFIX_0xde_SET_3_pHL: {
      cpu_set(3, mem_ref_u8(&emu->mem, reg_hl(&emu->cpu.reg)));
      return 16;
    }
    case OPCODE_PREFIX_0xdf_SET_3_A: {
      cpu_set(3, &emu->cpu.reg.a);
      return 8;
    }
    case OPCODE_PREFIX_0xe0_SET_4_B: {
      cpu_set(4, &emu->cpu.reg.b);
      return 8;
    }
    case OPCODE_PREFIX_0xe1_SET_4_C: {
      cpu_set(4, &emu->cpu.reg.c);
      return 8;
    }
    case OPCODE_PREFIX_0xe2_SET_4_D: {
      cpu_set(4, &emu->cpu.reg.d);
      return 8;
    }
    case OPCODE_PREFIX_0xe3_SET_4_E: {
      cpu_set(4, &emu->cpu.reg.e);
      return 8;
    }
    case OPCODE_PREFIX_0xe4_SET_4_H: {
      cpu_set(4, &emu->cpu.reg.h);
      return 8;
    }
    case OPCODE_PREFIX_0xe5_SET_4_L: {
      cpu_set(4, &emu->cpu.reg.l);
      return 8;
    }
    case OPCODE_PREFIX_0xe6_SET_4_pHL: {
      cpu_set(4, mem_ref_u8(&emu->mem, reg_hl(&emu->cpu.reg)));
      return 16;
    }
    case OPCODE_PREFIX_0xe7_SET_4_A: {
      cpu_set(4, &emu->cpu.reg.a);
      return 8;
    }
    case OPCODE_PREFIX_0xe8_SET_5_B: {
      cpu_set(5, &emu->cpu.reg.b);
      return 8;
    }
    case OPCODE_PREFIX_0xe9_SET_5_C: {
      cpu_set(5, &emu->cpu.reg.c);
      return 8;
    }
    case OPCODE_PREFIX_0xea_SET_5_D: {
      cpu_set(5, &emu->cpu.reg.d);
      return 8;
    }
    case OPCODE_PREFIX_0xeb_SET_5_E: {
      cpu_set(5, &emu->cpu.reg.e);
      return 8;
    }
    case OPCODE_PREFIX_0xec_SET_5_H: {
      cpu_set(5, &emu->cpu.reg.h);
      return 8;
    }
    case OPCODE_PREFIX_0xed_SET_5_L: {
      cpu_set(5, &emu->cpu.reg.l);
      return 8;
    }
    case OPCODE_PREFIX_0xee_SET_5_pHL: {
      cpu_set(5, mem_ref_u8(&emu->mem, reg_hl(&emu->cpu.reg)));
      return 16;
    }
    case OPCODE_PREFIX_0xef_SET_5_A: {
      cpu_set(5, &emu->cpu.reg.a);
      return 8;
    }
    case OPCODE_PREFIX_0xf0_SET_6_B: {
      cpu_set(6, &emu->cpu.reg.b);
      return 8;
    }
    case OPCODE_PREFIX_0xf1_SET_6_C: {
      cpu_set(6, &emu->cpu.reg.c);
      return 8;
    }
    case OPCODE_PREFIX_0xf2_SET_6_D: {
      cpu_set(6, &emu->cpu.reg.d);
      return 8;
    }
    case OPCODE_PREFIX_0xf3_SET_6_E: {
      cpu_set(6, &emu->cpu.reg.e);
      return 8;
    }
    case OPCODE_PREFIX_0xf4_SET_6_H: {
      cpu_set(6, &emu->cpu.reg.h);
      return 8;
    }
    case OPCODE_PREFIX_0xf5_SET_6_L: {
      cpu_set(6, &emu->cpu.reg.l);
      return 8;
    }
    case OPCODE_PREFIX_0xf6_SET_6_pHL: {
      cpu_set(6, mem_ref_u8(&emu->mem, reg_hl(&emu->cpu.reg)));
      return 16;
    }
    case OPCODE_PREFIX_0xf7_SET_6_A: {
      cpu_set(6, &emu->cpu.reg.a);
      return 8;
    }
    case OPCODE_PREFIX_0xf8_SET_7_B: {
      cpu_set(7, &emu->cpu.reg.b);
      return 8;
    }
    case OPCODE_PREFIX_0xf9_SET_7_C: {
      cpu_set(7, &emu->cpu.reg.c);
      return 8;
    }
    case OPCODE_PREFIX_0xfa_SET_7_D: {
      cpu_set(7, &emu->cpu.reg.d);
      return 8;
    }
    case OPCODE_PREFIX_0xfb_SET_7_E: {
      cpu_set(7, &emu->cpu.reg.e);
      return 8;
    }
    case OPCODE_PREFIX_0xfc_SET_7_H: {
      cpu_set(7, &emu->cpu.reg.h);
      return 8;
    }
    case OPCODE_PREFIX_0xfd_SET_7_L: {
      cpu_set(7, &emu->cpu.reg.l);
      return 8;
    }
    case OPCODE_PREFIX_0xfe_SET_7_pHL: {
      cpu_set(7, mem_ref_u8(&emu->mem, reg_hl(&emu->cpu.reg)));
      return 16;
    }
    case OPCODE_PREFIX_0xff_SET_7_A: {
      cpu_set(7, &emu->cpu.reg.a);
      return 8;
    }
    }
    __builtin_unreachable();
  }
  default: {
    FATAL("Cannot execute opcode 0x(%02x)\n", (uint8_t)opcode->kind);
  }
  }
  return 4;
}

void emu_loop(Emu *emu) {
  Opcode opcode;
  uint8_t cycle;

  while (true) {
    GAMEBOY_DOCTOR(emu);

    cycle = cpu_interrupt(&emu->cpu, &emu->mem);
    mem_tick(&emu->mem, cycle);

    if (emu->cpu.ei_waiting) {
      emu->cpu.ei_waiting = false;
      emu->cpu.ime = true;
    }

    decode(emu, &opcode);

    cycle = execute(emu, &opcode);
    // not halt, normal tick
    if (!emu->cpu.halted) {
      mem_tick(&emu->mem, cycle);
      continue;
    }

    // halt!
    while ((int)emu->cpu.halted && !cpu_has_interrupt_pending(&emu->mem)) {
      mem_tick(&emu->mem, 4);
    }
    emu->cpu.halted = false;
  }
}
