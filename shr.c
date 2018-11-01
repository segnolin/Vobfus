#include "vm.h"

void v_shr(v_register *v_reg)
{
  printf("shr\n");

  v_info *info = parse(v_reg, 2);

  if (info[0].SIZE == 0) {
    *(char *)info[0].operand >>= *(int *)info[1].operand;
  }
  else if (info[0].SIZE == 1) {
    *(short *)info[0].operand >>= *(int *)info[1].operand;
  }
  else if (info[0].SIZE == 2) {
    *(int *)info[0].operand >>= *(int *)info[1].operand;
  }
  else if (info[0].SIZE == 3) {
    *(long long *)info[0].operand >>= *(int *)info[1].operand;
  }

  free(info);
}
