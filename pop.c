#include "vm.h"

void v_pop(v_register *v_reg)
{
  printf("pop\n");

  v_info *info = parse(v_reg, 1);

  free(info);
}
