#include "vm.h"

void v_div(v_register *v_reg)
{
  printf("div\n");

  v_info *info = parse(v_reg, 2);

  free(info);
}
