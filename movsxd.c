#include "vm.h"

void v_movsxd(v_register *v_reg)
{
  printf("movsxd\n");

  v_movsx(v_reg);
}
