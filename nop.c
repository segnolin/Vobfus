#include "vm.h"

void v_nop(v_register *v_reg)
{
  printf("nop\n");

  v_info *info = parse(v_reg, 0);

  free(info);
}
