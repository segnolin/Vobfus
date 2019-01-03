#include "vm.h"

void v_call(v_register *v_reg)
{
  printf("call\n");

  v_info *info = parse(v_reg, 1);

  free(info);
}
