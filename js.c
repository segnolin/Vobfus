#include "vm.h"

void v_js(v_register *v_reg)
{
  printf("js\n");

  v_info *info = parse(v_reg, 1);

  if (v_reg->v_eflags & 0x80) {
    if (info[0].SIGN == 2) {
      v_reg->v_rip -= *(long long *)info[0].operand;
    }
    else {
      v_reg->v_rip += *(long long *)info[0].operand;
    }
  }

  free(info);
}
