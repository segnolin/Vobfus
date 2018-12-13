#include "vm.h"

void v_jge(v_register *v_reg)
{
  DEBUG("jge\n");

  v_info *info = parse(v_reg, 1);

  if ((v_reg->v_eflags & 0x80 == v_reg->v_eflags & 0x800) || (v_reg->v_eflags & 0x40)) {
    if (info[0].SIGN == 2) {
      v_reg->v_rip -= *(long long *)info[0].operand;
    }
    else {
      v_reg->v_rip += *(long long *)info[0].operand;
    }
  }

  free(info);
}
