#include "vm.h"

void v_cdqe(v_register *v_reg)
{
  printf("cdqe\n");

  v_info *info = parse(v_reg, 0);

  long long tmp = (long long)*(int *)&(v_reg->v_rax);

  v_reg->v_rax = tmp;

  free(info);
}
