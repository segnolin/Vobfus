#include "vm.h"

/*
 *
 *    [           ] rbp - 0x200 <-- rsp
 *    [           ]     :
 *    [           ]     :
 *    [   v_rax   ] rbp - 0xa0 <-- v_rbp, v_rsp
 *    [   v_rbx   ] rbp - 0x98
 *    [   v_rcx   ] rbp - 0x90
 *    [   v_rdx   ] rbp - 0x88
 *    [   v_rsi   ] rbp - 0x80
 *    [   v_rdi   ] rbp - 0x78
 *    [   v_rbp   ] rbp - 0x70
 *    [   v_rsp   ] rbp - 0x68
 *    [   v_rip   ] rbp - 0x60
 *    [   v_r8    ] rbp - 0x58
 *    [   v_r9    ] rbp - 0x50
 *    [   v_r10   ] rbp - 0x48
 *    [   v_r11   ] rbp - 0x40
 *    [   v_r12   ] rbp - 0x38
 *    [   v_r13   ] rbp - 0x30
 *    [   v_r14   ] rbp - 0x28
 *    [   v_r15   ] rbp - 0x20
 *    [   v_rsv   ] rbp - 0x18
 *    [  v_eflag  ] rbp - 0x10
 *    [  old_rbp  ] rbp - 0x8   <-- rbp, old_rsp
 *    [    ret    ]
 *
 */

void (*v_table[])(v_register *v_reg) = {
  v_mov,
  v_add,
  v_sub
};

char v_code[] = {'\x80', '\xda', '\x07', '\x14', '\x82', '\x06', '\x80', '\xda', '\x07', '\x18', '\x82', '\x05', '\x80', '\x82', '\x00', '\xda', '\x07', '\x14', '\x80', '\xda', '\x07', '\x08', '\x82', '\x00', '\x80', '\x82', '\x00', '\xda', '\x07', '\x18', '\x80', '\xda', '\x07', '\x04', '\x82', '\x00', '\x80', '\x82', '\x04', '\xda', '\x07', '\x08', '\x80', '\x82', '\x00', '\xda', '\x07', '\x04', '\x81', '\x82', '\x00', '\x82', '\x04', '\x00'};

int volatile func(int x, int y)
{
  __asm__
    (
     ".intel_syntax noprefix;"
     "construct:;"
     "  sub rsp, 0x200;"                      // set rsp to lower place
     "  lea rax, [rbp - 0xa0];"
     "  mov qword ptr [rbp - 0x70], rax;"     // set v_rbp
     "  mov qword ptr [rbp - 0x68], rax;"     // set v_rsp
     "  lea rax, %0;"
     "  mov qword ptr [rbp - 0x60], rax;"     // set v_rip
     "  mov qword ptr [rbp - 0x78], rdi;"     // set v_rdi
     "  mov qword ptr [rbp - 0x80], rsi;"     // set v_rsi
     "fetch:;"
     "  xor rax, rax;"
     "  mov rbx, qword ptr [rbp - 0x60];"
     "  mov al, byte ptr [rbx];"
     "  test al, al;"                         // test if next opcode is \x00
     "  je destruct;"
     "execute:;"
     "  lea rdi, [rbp - 0xa0];"
     "  sub rax, 0x80;"
     "  shl rax, 3;"
     "  lea rbx, %1;"
     "  add rbx, rax;"                        // use rax as v_table offset
     "  call qword ptr [rbx];"                // call function accordingly
     "  jmp fetch;"
     "destruct:;"
     "  mov rax, qword ptr [rbp - 0xa0];"    // return value
     "  mov rsp, rbp;"                        // restore rsp
     :
     : "m" (v_code), "m" (v_table)
     :
    );
}

int main(void)
{
  int x, y;
  printf("x: ");
  scanf("%d", &x);
  printf("y: ");
  scanf("%d", &y);
  printf("result: %d\n", func(x, y));
  return 0;
}
