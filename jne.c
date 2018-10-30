#include "vm.h"

void v_jne(v_register *v_reg)
{
  printf("jne\n");

  int idx = 0;
  char v_code[16];

  // skip inst byte
  v_code[idx++] = *(char *)(v_reg->v_rip);
  v_reg->v_rip++;

  bool REG;
  bool SIGN;
  int  IMM;
  bool REF;
  bool RSV;
  int  SIZE;

  long long *operand;

  // get opX_meta
  char meta = *(char *)(v_reg->v_rip);

  // skip opX_meta byte
  v_code[idx++] = *(char *)(v_reg->v_rip);
  v_reg->v_rip++;

  REG  = (meta & 0x80) >> 7;
  SIGN = (meta & 0x40) >> 6;
  IMM  = (meta & 0x30) >> 4;
  REF  = (meta & 0x8) >> 3;
  RSV  = (meta & 0x4) >> 2;
  SIZE = (meta & 0x3) >> 0;

  printf("op0_REG: %d\n", REG);
  printf("op0_SIGN: %d\n", SIGN);
  printf("op0_IMM: %d\n", IMM);
  printf("op0_REF: %d\n", REF);
  printf("op0_RSV: %d\n", RSV);
  printf("op0_SIZE: %d\n", SIZE);

  // check reference
  if (REF) {
    char reg = *(char *)(v_reg->v_rip);
    v_code[idx++] = reg;
    v_reg->v_rip++;

    operand = (long long *)v_reg;
    operand += reg;
    operand = (long long *)*operand;

    // check offset
    if (IMM) {
      int imm_size = (1 << IMM) - 1;
      char imm[8] = {0};
      strncpy(&v_code[idx], (char *)(v_reg->v_rip), imm_size);
      strncpy(imm, (char *)(v_reg->v_rip), imm_size);
      v_reg->v_rip += imm_size;
      idx += imm_size;

      if (SIGN) {
        operand = (long long *)((char *)operand - *(long long *)imm);
      }
      else {
        operand = (long long *)((char *)operand + *(long long *)imm);
      }
    }
  }

  // register or immediate
  else {

    // register
    if (REG) {
      char reg = *(char *)(v_reg->v_rip);
      v_code[idx++] = reg;
      v_reg->v_rip++;

      operand = (long long *)v_reg;
      operand += reg;
    }

    // immediate
    else {
      int imm_size = (1 << IMM) - 1;
      char imm[8] = {0};
      strncpy(&v_code[idx], (char *)(v_reg->v_rip), imm_size);
      strncpy(imm, (char *)(v_reg->v_rip), imm_size);
      v_reg->v_rip += imm_size;
      idx += imm_size;

      operand = (long long *)imm;
    }
  }
  printf("\n");

  if (!(v_reg->v_eflags & 0x40)) {
    printf("jumpppp\n");
    if (SIGN) {
      v_reg->v_rip -= *operand;
    }
    else {
      v_reg->v_rip += *operand;
    }
  }
  printf("end\n");

  for (int i = 0; i < idx; i++) {
    printf("0x%02x ", v_code[i] & 0xff);
  }
  printf("\n");
}
