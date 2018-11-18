#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct v_register
{
  long long v_rax;
  long long v_rbx;
  long long v_rcx;
  long long v_rdx;
  long long v_rsi;
  long long v_rdi;
  long long v_rbp;
  long long v_rsp;
  long long v_rip;
  long long v_r8;
  long long v_r9;
  long long v_r10;
  long long v_r11;
  long long v_r12;
  long long v_r13;
  long long v_r14;
  long long v_r15;
  long long v_rsv;
  long long v_eflags;
} v_register;

typedef struct v_info
{
  int TYPE;
  int SIGN;
  bool REF;
  int SIZE;
  long long operand;
} v_info;

v_info *parse(v_register *v_reg, int size);

void v_nop(v_register *v_reg);

void v_push(v_register *v_reg);

void v_pop(v_register *v_reg);

void v_mov(v_register *v_reg);

void v_movsx(v_register *v_reg);

void v_movsxd(v_register *v_reg);

void v_movzx(v_register *v_reg);

void v_cdqe(v_register *v_reg);

void v_lea(v_register *v_reg);

void v_add(v_register *v_reg);

void v_sub(v_register *v_reg);

void v_xor(v_register *v_reg);

void v_jmp(v_register *v_reg);

void v_je(v_register *v_reg);

void v_jne(v_register *v_reg);

void v_jg(v_register *v_reg);

void v_jge(v_register *v_reg);

void v_ja(v_register *v_reg);

void v_jae(v_register *v_reg);

void v_jl(v_register *v_reg);

void v_jle(v_register *v_reg);

void v_jb(v_register *v_reg);

void v_jbe(v_register *v_reg);

void v_jo(v_register *v_reg);

void v_jno(v_register *v_reg);

void v_jz(v_register *v_reg);

void v_jnz(v_register *v_reg);

void v_js(v_register *v_reg);

void v_jns(v_register *v_reg);

void v_shl(v_register *v_reg);

void v_shr(v_register *v_reg);

void v_cmp(v_register *v_reg);

void v_test(v_register *v_reg);

void v_call(v_register *v_reg);

void v_div(v_register *v_reg);

void v_imul(v_register *v_reg);
