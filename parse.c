#include "vm.h"

v_info *parse(v_register *v_reg, int size)
{
  v_info *info;
  info = malloc(size * sizeof(v_info));

  int idx = 0;
  char v_code[16];

  //skip inst byte
  v_code[idx++] = *(char *)(v_reg->v_rip);
  v_reg->v_rip++;

  for (int i = 0; i < size; i++) {

    // skip opX_meta byte
    char meta = *(char *)(v_reg->v_rip);
    v_code[idx++] = meta;
    v_reg->v_rip++;

    info[i].TYPE = (meta & 0xe0) >> 5;
    info[i].SIGN = (meta & 0x18) >> 3;
    info[i].REF  = (meta & 0x4) >> 2;
    info[i].SIZE = (meta & 0x3);

    DEBUG("op%d_TYPE: %d\n", i, info[i].TYPE);
    DEBUG("op%d_SIGN: %d\n", i, info[i].SIGN);
    DEBUG("op%d_REF:  %d\n", i, info[i].REF);
    DEBUG("op%d_SIZE: %d\n", i, info[i].SIZE);
    DEBUG("\n");

    // immediate only
    if (info[i].TYPE <= 2) {
      int avail_size[] = {1, 3, 8};
      int imm_size = avail_size[info[i].TYPE];
      char imm[8] = {0};
      strncpy(&v_code[idx], (char *)(v_reg->v_rip), imm_size);
      strncpy(imm, (char *)(v_reg->v_rip), imm_size);
      v_reg->v_rip += imm_size;
      idx += imm_size;

      info[i].operand = (long long)imm;
    }
    // reference
    else if (info[i].REF) {
      // first register
      char reg = *(char *)(v_reg->v_rip);
      v_code[idx++] = reg;
      v_reg->v_rip++;

      info[i].operand = (long long)v_reg;
      int mul = 0;
      if ((reg & 0xff) > 0x11) {
        mul = (reg & 0xc0) >> 6;
        reg &= 0x3f;
      }
      info[i].operand += reg * 8;
      info[i].operand = *(long long *)(info[i].operand);
      info[i].operand <<= mul;

      // check second register
      if (info[i].TYPE == 4 || info[i].TYPE == 7) {
        long long tmp;
        char reg = *(char *)(v_reg->v_rip);
        v_code[idx++] = reg;
        v_reg->v_rip++;

        tmp = (long long)v_reg;
        int mul = 0;
        if ((reg & 0xff) > 0x11) {
          mul = (reg & 0xc0) >> 6;
          reg &= 0x3f;
        }
        tmp += reg * 8;
        tmp = *(long long *)tmp;
        tmp <<= mul;
        if (info[i].SIGN & 0x2) {
          info[i].operand -= tmp;
        }
        else {
          info[i].operand += tmp;
        }
      }

      // check immediate
      if (info[i].TYPE == 5 || info[i].TYPE == 6) {
        int avail_size[] = {1, 3};
        int imm_size = avail_size[info[i].TYPE - 5];
        char imm[8] = {0};
        strncpy(&v_code[idx], (char *)(v_reg->v_rip), imm_size);
        strncpy(imm, (char *)(v_reg->v_rip), imm_size);
        v_reg->v_rip += imm_size;
        idx += imm_size;

        if (info[i].SIGN & 0x2) {
          info[i].operand -= *(long long *)imm;
        }
        else {
          info[i].operand += *(long long *)imm;
        }
      }

      else if (info[i].TYPE == 7){
        char imm[8] = {0};
        strncpy(&v_code[idx], (char *)(v_reg->v_rip), 1);
        strncpy(imm, (char *)(v_reg->v_rip), 1);
        v_reg->v_rip++;
        idx++;

        if (info[i].SIGN & 0x1) {
          info[i].operand -= *(long long *)imm;
        }
        else {
          info[i].operand += *(long long *)imm;
        }
      }
    }
    // register only
    else {
      char reg = *(char *)(v_reg->v_rip);
      v_code[idx++] = reg;
      v_reg->v_rip++;

      info[i].operand = (long long)v_reg;
      info[i].operand += reg * 8;
    }
  }

  // print v_code
  for (int i = 0; i < idx; i++) {
    DEBUG("0x%02x ", v_code[i] & 0xff);
  }
  DEBUG("\n");

  return info;
}
