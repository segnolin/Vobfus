#include "vm.h"

void v_movsxd(v_register *v_reg)
{
  DEBUG("movsxd\n");

  v_movsx(v_reg);
}
