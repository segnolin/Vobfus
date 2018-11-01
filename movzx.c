#include "vm.h"

void v_movzx(v_register *v_reg)
{
  printf("movzx\n");

  v_info *info = parse(v_reg, 2);

  long long tmp = *(long long *)info[1].operand;

  if (info[1].SIZE == 0) {
    tmp = (long long)*(char *)&tmp & 0xff;
  }
  else if (info[1].SIZE == 1) {
    tmp = (long long)*(short *)&tmp & 0xffff;
  }
  else if (info[1].SIZE == 2) {
    tmp = (long long)*(int *)&tmp & 0xffffffff;
  }

  if (info[0].SIZE == 0) {
    *(char *)info[0].operand = *(char *)&tmp;
  }
  else if (info[0].SIZE == 1) {
    *(short *)info[0].operand = *(short *)&tmp;
  }
  else if (info[0].SIZE == 2) {
    *(int *)info[0].operand = *(int *)&tmp;
  }
  else if (info[0].SIZE == 3) {
    *(long long *)info[0].operand = *(long long *)&tmp;
  }

  free(info);
}
