from capstone import *
from pwn import *

import re
import string
import subprocess
import sys

reg_table = {
    'rax': '\x00',
    'rbx': '\x01',
    'rcx': '\x02',
    'rdx': '\x03',
    'rsi': '\x04',
    'rdi': '\x05',
    'rbp': '\x06',
    'rsp': '\x07',
    'rip': '\x08',
    'r8' : '\x09',
    'r9' : '\x0a',
    'r10': '\x0b',
    'r11': '\x0c',
    'r12': '\x0d',
    'r13': '\x0e',
    'r14': '\x0f',
    'r15': '\x10',
    'rsv': '\x11',
}

inst_table = {
    'mov'   : '\x80',
    'movsx' : '\x81',
    'movsxd': '\x82',
    'movzx' : '\x83',
    'add'   : '\x84',
    'sub'   : '\x85',
    'xor'   : '\x86',
    'jmp'   : '\x87',
    'jne'   : '\x88',
    'shl'   : '\x89',
    'test'  : '\x8a'
}

vaddr = 0

class Code:

    def __init__(self, addr, opcode, operand):

        self.addr = addr
        self.opcode = opcode
        self.operand = operand
        self.vcode = ''
        self.vaddr = 0

        self.REG  = []
        self.SIGN = []
        self.IMM  = []
        self.REF  = []
        self.RSV  = []
        self.SIZE = []
        self.reg  = []
        self.imm  = []

    def print_code(self):

        print '0x{:x}:\t{}\t{}'.format(self.addr, self.opcode, self.operand)
        print ''

    def virtualizatoin(self):

        '''
        Vcode Format

        [inst] [op0_meta] [op0_data] [op1_meta] [op1_data]

        [inst]: 1 byte
        [opX_meta]: 1 byte [reg] [sign] [imm] [ref] [rsv] [size]
            [reg]:  1 bit
            [sign]: 1 bit
            [imm]:  2 bit
            [ref]:  1 bit
            [rsv]:  1 bit
            [size]: 2 bit
        [opX_data]: 1~8 byte
        '''

        # [inst]
        self.vcode += inst_table[self.opcode]

        # [opX_meta]
        ops = re.split(r', ', self.operand)
        for op in ops:

            print '< {} >'.format(op)

            REG  = 0b0
            SIGN = 0b0
            IMM  = 0b00
            REF  = 0b0
            RSV  = 0b0
            SIZE = 0b00

            reg = '\x00'
            imm = 0

            # reference (always have register)
            if '[' in op:

                # [reg] [ref]
                REG = 0b1
                REF = 0b1
                op = op.replace('[', '').replace(']', '')

                # content
                if 'ptr' in op:
                    comps = re.split(r' ', op)

                    # set register
                    reg = reg_table[comps[2]]
                    self.reg.append(reg)

                    # [size]
                    if comps[0] == 'qword':
                        SIZE = 0b11
                    elif comps[0] == 'dword':
                        SIZE = 0b10
                    elif comps[0] == 'word':
                        SIZE = 0b01
                    elif comps[0] == 'byte':
                        SIZE = 0b00
                    else:
                        print 'size error'
                        sys.exit(1)

                    # check offset
                    if len(comps) == 5:

                        # [sign]
                        if comps[3] == '-':
                            SIGN = 0b1

                        # [imm]
                        imm = comps[4].replace('0x', '')
                        if len(imm) <= 2:
                            IMM = 0b01
                        elif len(imm) <= 6:
                            IMM = 0b10
                        elif len(imm) <= 14:
                            IMM = 0b11
                        else:
                            print 'imm error'
                            sys.exit(1)

                        # set immediate
                        imm = int(imm, 16)
                        self.imm.append(imm)

                # for lea
                else:
                    pass

            # register or immediate
            else:

                # check immediate 
                if op[0].isdigit():

                    # [imm]
                    imm = op.replace('0x', '')
                    if len(imm) <= 2:
                        IMM = 0b01
                    elif len(imm) <= 6:
                        IMM = 0b10
                    elif len(imm) <= 14:
                        IMM = 0b11
                    else:
                        print 'imm error'

                    # set immediate
                    imm = int(imm, 16)
                    self.imm.append(imm)

                # check register
                else:

                    # [reg]
                    REG = 0b1
                    
                    # [size]
                    if op.startswith('r'):
                        SIZE = 0b11
                    elif op.startswith('e'):
                        SIZE = 0b10
                        op = op.replace('e', 'r')
                    elif op.endswith('x'):
                        SIZE = 0b01
                        op = 'r' + op
                    elif op.endswith('l'):
                        SIZE = 0b00
                        op = 'r' + op.replace('l', 'x')
                    else:
                        print 'size error'
                        sys.exit(1)

                    # set register
                    reg = reg_table[op.decode('utf-8')]
                    self.reg.append(reg)

            print 'REG:    {:b}'.format(REG)
            print 'SIGN:   {:b}'.format(SIGN)
            print 'IMM:    {0:02b}'.format(IMM)
            print 'REF:    {:b}'.format(REF)
            print 'RSV:    {:b}'.format(RSV)
            print 'SIZE:   {0:02b}'.format(SIZE)
            print 'reg:    0x{:x}'.format(ord(reg))
            print 'imm:    0x{:x}'.format(imm)
            print ''

            self.REG.append(REG)
            self.SIGN.append(SIGN)
            self.IMM.append(IMM)
            self.REF.append(REF)
            self.RSV.append(RSV)
            self.SIZE.append(SIZE)

            # append opX_meta to vcode
            meta = chr((REG << 7) + (SIGN << 6) + (IMM << 4) + (REF << 3) + (RSV << 2) + SIZE)
            self.vcode += meta

            # append opX_data to vcode
            if REG:
                self.vcode += reg
            if IMM:
                imml = 2 ** IMM - 1
                self.vcode += p64(imm)[:imml]

        # set virtual address
        global vaddr
        self.vaddr = vaddr
        vaddr += len(self.vcode)

    def print_vcode(self):

        print '0x{:x}:'.format(self.vaddr),
        for c in self.vcode:
            print '0x{:02x}'.format(ord(c)),

        print ''
        print '=================================================='


def disassemble(exec_name, func_start, func_end):

    func_size = func_end - func_start + 1

    with open(exec_name, 'rb') as f:
        f.seek(func_start)
        binary = f.read(func_size)

    codes = []
    md = Cs(CS_ARCH_X86, CS_MODE_64)
    for inst in md.disasm(binary, func_start):
        codes.append(Code(inst.address, inst.mnemonic, inst.op_str))

    return codes

def translate(exec_name, func_start, func_end):

    codes = disassemble(exec_name, func_start, func_end)
    for code in codes:
        code.print_code()
        code.virtualizatoin()
        code.print_vcode()

    # jump relocation
    for i in range(len(codes)):
        if codes[i].opcode.startswith('j') and codes[i].REG[0] == 0b0:
            tar_addr = codes[i].imm[0]
            tar_vaddr = 0
            for j in range(len(codes)):
                if codes[j].addr == tar_addr:
                    tar_vaddr = codes[j].vaddr

            # modify vcode
            curr_vaddr = codes[i].vaddr + len(codes[i].vcode)
            mod_vcode = ''
            if tar_vaddr > curr_vaddr:
                offset = tar_vaddr - curr_vaddr
                mod_vcode = codes[i].vcode[:2] + p64(offset)[:3]
            else:
                offset = curr_vaddr - tar_vaddr
                mod_vcode = codes[i].vcode[:1] + '\x60' + p64(offset)[:3]
            codes[i].vcode = mod_vcode

    # output
    vcode = ''
    for code in codes:
        vcode += code.vcode

    output = 'char v_code[] = {'
    for i in range(len(vcode)):
        output += "'\\x{:02x}', ".format(ord(vcode[i]))
    output += "'\\x00'};"

    print output

def get_func_addr(exec_name, func_name):

    cmd = 'gdb -batch -ex "file {}" -ex "disassemble {}"'.format(exec_name, func_name)
    output = subprocess.check_output(cmd, shell=True).splitlines()

    func_start = int(output[1].split()[0], 16) - 0x400000
    func_end = int(output[-2].split()[0],16) - 0x400000

    return func_start, func_end

if __name__ == '__main__':

    if len(sys.argv) != 3:
        print 'usage: python x64_trans.py [exec_name] [func_name]'
        exit()

    exec_name = sys.argv[1]
    func_name = sys.argv[2]

    # get function address range
    func_start, func_end = get_func_addr(exec_name, func_name)

    # skip function prologue & function epilogue
    func_start += 4
    func_end -= 2

    translate(exec_name, func_start, func_end)
