#include "vm.h"

void v_sub(v_register *v_reg)
{
  printf("sub\n");

  int idx = 0;
  char v_code[16];

  // skip inst byte
  v_code[idx++] = *(char *)(v_reg->v_rip);
  v_reg->v_rip++;

  bool REG[2];
  bool SIGN[2];
  int  IMM[2];
  bool REF[2];
  bool RSV[2];
  int  SIZE[2];

  long long *operand[2];

  for (int i = 0; i < 2; i++) {

    // get opX_meta
    char meta = *(char *)(v_reg->v_rip);

    // skip opX_meta byte
    v_code[idx++] = *(char *)(v_reg->v_rip);
    v_reg->v_rip++;

    REG[i]  = (meta & 0x80) >> 7;
    SIGN[i] = (meta & 0x40) >> 6;
    IMM[i]  = (meta & 0x30) >> 4;
    REF[i]  = (meta & 0x8) >> 3;
    RSV[i]  = (meta & 0x4) >> 2;
    SIZE[i] = (meta & 0x3) >> 0;

    printf("op%d_REG: %d\n", i, REG[i]);
    printf("op%d_SIGN: %d\n", i, SIGN[i]);
    printf("op%d_IMM: %d\n", i, IMM[i]);
    printf("op%d_REF: %d\n", i, REF[i]);
    printf("op%d_RSV: %d\n", i, RSV[i]);
    printf("op%d_SIZE: %d\n", i, SIZE[i]);

    // check reference
    if (REF[i]) {
      char reg = *(char *)(v_reg->v_rip);
      v_code[idx++] = reg;
      v_reg->v_rip++;

      operand[i] = (long long *)v_reg;
      operand[i] += reg;
      operand[i] = (long long *)*operand[i];

      // check offset
      if (IMM[i]) {
        int imm_size = (1 << IMM[i]) - 1;
        char imm[8] = {0};
        strncpy(&v_code[idx], (char *)(v_reg->v_rip), imm_size);
        strncpy(imm, (char *)(v_reg->v_rip), imm_size);
        v_reg->v_rip += imm_size;
        idx += imm_size;

        if (SIGN) {
          operand[i] = (long long *)((char *)operand[i] - *(long long *)imm);
        }
        else {
          operand[i] = (long long *)((char *)operand[i] + *(long long *)imm);
        }
      }
    }

    // register or immediate
    else {

      // register
      if (REG[i]) {
        char reg = *(char *)(v_reg->v_rip);
        v_code[idx++] = reg;
        v_reg->v_rip++;

        operand[i] = (long long *)v_reg;
        operand[i] += reg;
      }

      // immediate
      else {
        int imm_size = (1 << IMM[i]) - 1;
        char imm[8] = {0};
        strncpy(&v_code[idx], (char *)(v_reg->v_rip), imm_size);
        strncpy(imm, (char *)(v_reg->v_rip), imm_size);
        v_reg->v_rip += imm_size;
        idx += imm_size;

        operand[i] = (long long *)imm;
      }
    }
    printf("\n");
  }

  if (SIZE[0] == 0) {
    *(char *)operand[0] -= *(char *)operand[1];
  }
  else if (SIZE[0] == 1) {
    *(short *)operand[0] -= *(short *)operand[1];
  }
  else if (SIZE[0] == 2) {
    *(int *)operand[0] -= *(int *)operand[1];
  }
  else if (SIZE[0] == 3) {
    *(long long *)operand[0] -= *(long long *)operand[1];
  }

  for (int i = 0; i < idx; i++) {
    printf("0x%02x ", v_code[i] & 0xff);
  }
  printf("\n");
}
