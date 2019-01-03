#include "vm.h"

void v_imul(v_register *v_reg)
{
  printf("imul\n");

  v_info *info = parse(v_reg, 2);

  free(info);
}
