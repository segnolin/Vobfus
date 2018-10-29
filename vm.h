#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct v_register
{
  long v_rax;
  long v_rbx;
  long v_rcx;
  long v_rdx;
  long v_rsi;
  long v_rdi;
  long v_rbp;
  long v_rsp;
  long v_rip;
  long v_r8;
  long v_r9;
  long v_r10;
  long v_r11;
  long v_r12;
  long v_r13;
  long v_r14;
  long v_r15;
  long v_rsv;
  long v_eflags;
} v_register;

void v_mov(v_register *v_reg);

void v_add(v_register *v_reg);

void v_sub(v_register *v_reg);

void v_xor(v_register *v_reg);
