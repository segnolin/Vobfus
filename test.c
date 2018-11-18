#include "vm.h"

void v_test(v_register *v_reg)
{
  printf("test\n");

  v_info *info = parse(v_reg, 2);

  int PF;
  int ZF;
  int SF;

  if (info[0].SIZE == 0) {
    char result = *(char *)info[0].operand & *(char *)info[1].operand;
    SF = (result & (1 << 7)) >> 7;
    ZF = (result) ? 0 : 1;
    for (int i = 0; i < 8; i++) {
      PF ^= (result & (1 << i)) >> i;
    }
  }
  else if (info[0].SIZE == 1) {
    short result = *(short *)info[0].operand & *(short *)info[1].operand;
    SF = (result & (1 << 15)) >> 15;
    ZF = (result) ? 0 : 1;
    for (int i = 0; i < 16; i++) {
      PF ^= (result & (1 << i)) >> i;
    }
  }
  else if (info[0].SIZE == 2) {
    int result = *(int *)info[0].operand & *(int *)info[1].operand;
    SF = (result & (1 << 31)) >> 31;
    ZF = (result) ? 0 : 1;
    for (int i = 0; i < 32; i++) {
      PF ^= (result & (1 << i)) >> i;
    }
  }
  else if (info[0].SIZE == 3) {
    long long result = *(long long *)info[0].operand & *(long long *)info[1].operand;
    SF = (result & (1LL << 63)) >> 63;
    ZF = (result) ? 0 : 1;
    for (int i = 0; i < 64; i++) {
      PF ^= (result & (1 << i)) >> i;
    }
  }

  // set CF and OF to 0
  v_reg->v_eflags &= ~0x801;

  // set PF
  if (PF) {
    v_reg->v_eflags |= 0x4;
  }
  else {
    v_reg->v_eflags &= ~0x4;
  }

  // set ZF
  if (ZF) {
    v_reg->v_eflags |= 0x40;
  }
  else {
    v_reg->v_eflags &= ~0x40;
  }

  // set SF
  if (SF) {
    v_reg->v_eflags |= 0x80;
  }
  else {
    v_reg->v_eflags &= ~0x80;
  }

  free(info);
}
