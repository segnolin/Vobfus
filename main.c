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
  v_nop,
  v_push,
  v_pop,
  v_mov,
  v_movsx,
  v_movsxd,
  v_movzx,
  v_cdqe,
  v_lea,
  v_add,
  v_sub,
  v_xor,
  v_jmp,
  v_je,
  v_jne,
  v_jg,
  v_jge,
  v_ja,
  v_jae,
  v_jl,
  v_jle,
  v_jb,
  v_jbe,
  v_jo,
  v_jno,
  v_jz,
  v_jnz,
  v_js,
  v_jns,
  v_shl,
  v_shr,
  v_cmp,
  v_test,
  v_call,
  v_div,
  v_imul
};

char v_code[] = {'\x83', '\xb7', '\x06', '\x18', '\x63', '\x05', '\x83', '\xb6', '\x06', '\x1c', '\x62', '\x04', '\x83', '\xb6', '\x06', '\x04', '\x00', '\x01', '\x8c', '\x20', '\xde', '\x00', '\x00', '\x83', '\x62', '\x00', '\xb6', '\x06', '\x04', '\x87', '\x88', '\x63', '\x03', '\x67', '\x80', '\x83', '\x63', '\x00', '\xb7', '\x06', '\x18', '\x89', '\x63', '\x00', '\x63', '\x03', '\x83', '\x62', '\x00', '\x66', '\x00', '\x83', '\xb6', '\x06', '\x0c', '\x62', '\x00', '\x83', '\x62', '\x00', '\xb6', '\x06', '\x04', '\x8a', '\x62', '\x00', '\x00', '\x01', '\x83', '\xb6', '\x06', '\x08', '\x62', '\x00', '\x8c', '\x20', '\x47', '\x00', '\x00', '\x83', '\x62', '\x00', '\xb6', '\x06', '\x08', '\x87', '\x88', '\x63', '\x03', '\x67', '\x80', '\x83', '\x63', '\x00', '\xb7', '\x06', '\x18', '\x89', '\x63', '\x00', '\x63', '\x03', '\x83', '\x62', '\x03', '\xb6', '\x06', '\x08', '\x85', '\x63', '\x03', '\x62', '\x03', '\x89', '\x63', '\x03', '\x00', '\x01', '\x88', '\x63', '\x02', '\x67', '\x83', '\x83', '\x63', '\x03', '\xb7', '\x06', '\x18', '\x89', '\x63', '\x03', '\x63', '\x02', '\x83', '\x62', '\x00', '\x66', '\x00', '\x83', '\x66', '\x03', '\x62', '\x00', '\x8a', '\xb6', '\x06', '\x08', '\x00', '\x01', '\x9f', '\xb6', '\x06', '\x08', '\x00', '\x00', '\x9b', '\x20', '\x27', '\x00', '\x00', '\x83', '\x62', '\x00', '\xb6', '\x06', '\x08', '\x87', '\x88', '\x63', '\x03', '\x67', '\x80', '\x83', '\x63', '\x00', '\xb7', '\x06', '\x18', '\x89', '\x63', '\x00', '\x63', '\x03', '\x83', '\x62', '\x00', '\x66', '\x00', '\x9f', '\xb6', '\x06', '\x0c', '\x62', '\x00', '\x93', '\x30', '\x79', '\x00', '\x00', '\x83', '\x62', '\x00', '\xb6', '\x06', '\x08', '\x87', '\x89', '\x63', '\x00', '\x00', '\x01', '\x88', '\x63', '\x03', '\x67', '\x80', '\x83', '\x63', '\x00', '\xb7', '\x06', '\x18', '\x89', '\x63', '\x03', '\x63', '\x00', '\x83', '\x62', '\x00', '\xb6', '\x06', '\x0c', '\x83', '\x66', '\x03', '\x62', '\x00', '\x89', '\xb6', '\x06', '\x04', '\x00', '\x01', '\x83', '\x62', '\x00', '\xb6', '\x06', '\x04', '\x9f', '\x62', '\x00', '\xb6', '\x06', '\x1c', '\x93', '\x30', '\xef', '\x00', '\x00', '\x80', '\x00'};

void volatile insertionSort(int arr[], int n)
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
     "  mov rax, qword ptr [rbp - 0xa0];"     // return value
     "  mov rsp, rbp;"                        // restore rsp
     :
     : "m" (v_code), "m" (v_table)
     :
    );
}

void printArray(int arr[], int n)
{
  int i;
  for (i = 0; i < n; i++)
    printf("%d ", arr[i]);
  printf("\n");
}

int main()
{
  int arr[] = {1524, 5772, 9906, 9432, 5306, 5762, 4371, 9723, 9262, 9163, 548, 3736, 3436, 7444, 9943, 811, 4215, 4729, 2628, 4105, 4037, 4083, 5397, 8664, 9460, 1784, 3785, 3067, 9358, 7312, 8307, 1937, 4282, 3470, 5392, 9720, 4800, 4886, 8412, 7641, 9326, 3347, 2090, 4488, 9799, 7541, 316, 9869, 2501, 4003, 9095, 784, 1351, 1507, 8407, 5526, 9557, 5974, 9651, 5200, 4325, 2102, 9228, 6673, 4274, 7979, 5030, 2421, 3103, 9062, 4841, 2096, 3501, 8380, 2422, 6122, 6033, 855, 6767, 4297, 8859, 7955, 176, 8883, 5556, 4988, 6100, 6091, 9274, 8378, 7696, 8531, 3927, 8836, 6839, 3592, 2739, 3150, 7955, 2887, 5002, 3536, 5918, 627, 5063, 6466, 7177, 5152, 5581, 4314, 1617, 8122, 4920, 3178, 7696, 4357, 6206, 2738, 529, 6669, 5035, 7676, 1470, 616, 1784, 2596, 6340, 1917, 7703, 7437, 9938, 7014, 8238, 5793, 758, 9369, 6611, 8693, 4445, 2894, 7870, 9801, 2581, 3915, 938, 184, 8947, 329, 9964, 2499, 8741, 3345, 8551, 7111, 3362, 1029, 3947};
  int n = sizeof(arr) / sizeof(arr[0]);

  insertionSort(arr, n);
  printArray(arr, n);

  return 0;
}
