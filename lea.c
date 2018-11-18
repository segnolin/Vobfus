#include "vm.h"

void v_lea(v_register *v_reg)
{
  printf("lea\n");

  v_info *info = parse(v_reg, 2);

  if (info[0].SIZE == 2) {
    *(int *)info[0].operand = (int)info[1].operand;
  }
  else if (info[0].SIZE == 3) {
    *(long long *)info[0].operand = (long long)info[1].operand;
  }

  free(info);
}
