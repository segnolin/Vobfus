#include "vm.h"

void v_push(v_register *v_reg)
{
  printf("push\n");

  v_info *info = parse(v_reg, 1);

  free(info);
}
