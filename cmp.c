#include "vm.h"

void v_cmp(v_register *v_reg)
{
  DEBUG("cmp\n");

  v_info *info = parse(v_reg, 2);

  int CF;
  int PF;
  int ZF;
  int SF;
  int OF;

  if (info[0].SIZE == 0) {
    char result = *(char *)info[0].operand - *(char *)info[1].operand;
    SF = (result & (1 << 7))? 1: 0;
    ZF = (result) ? 0 : 1;
    if (ZF) CF = 0;
    for (int i = 0; i < 8; i++) {
      PF ^= (result & (1 << i)) >> i;
    }
    if (*(char *)info[0].operand < *(char *)info[1].operand) {
      CF = 1;
      OF = SF ^ 1;
    }
    else if (*(char *)info[0].operand > *(char *)info[1].operand) {
      CF = 0;
      OF = SF;
    }
  }
  else if (info[0].SIZE == 1) {
    short result = *(short *)info[0].operand - *(short *)info[1].operand;
    SF = (result & (1 << 15))? 1: 0;
    ZF = (result) ? 0 : 1;
    if (ZF) CF = 0;
    for (int i = 0; i < 16; i++) {
      PF ^= (result & (1 << i)) >> i;
    }
    if (*(short *)info[0].operand < *(short *)info[1].operand) {
      CF = 1;
      OF = SF ^ 1;
    }
    else if (*(short *)info[0].operand > *(short *)info[1].operand) {
      CF = 0;
      OF = SF;
    }
  }
  else if (info[0].SIZE == 2) {
    int result = *(int *)info[0].operand - *(int *)info[1].operand;
    SF = (result & (1 << 31))? 1: 0;
    ZF = (result) ? 0 : 1;
    if (ZF) CF = 0;
    for (int i = 0; i < 32; i++) {
      PF ^= (result & (1 << i)) >> i;
    }
    if (*(int *)info[0].operand < *(int *)info[1].operand) {
      CF = 1;
      OF = SF ^ 1;
    }
    else if (*(int *)info[0].operand > *(int *)info[1].operand) {
      CF = 0;
      OF = SF;
    }
  }
  else if (info[0].SIZE == 3) {
    long long result = *(long long *)info[0].operand - *(long long *)info[1].operand;
    SF = (result & (1LL << 63))? 1: 0;
    ZF = (result) ? 0 : 1;
    if (ZF) CF = 0;
    for (int i = 0; i < 64; i++) {
      PF ^= (result & (1 << i)) >> i;
    }
    if (*(long long *)info[0].operand < *(long long *)info[1].operand) {
      CF = 1;
      OF = SF ^ 1;
    }
    else if (*(long long *)info[0].operand > *(long long *)info[1].operand) {
      CF = 0;
      OF = SF;
    }
  }

  // set CF
  if (CF) {
    v_reg->v_eflags |= 0x1;
  }
  else {
    v_reg->v_eflags &= ~0x1;
  }

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

  // set OF
  if (OF) {
    v_reg->v_eflags |= 0x800;
  }
  else {
    v_reg->v_eflags &= ~0x800;
  }

  free(info);
}
