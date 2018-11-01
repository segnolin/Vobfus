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
    'nop'   : '',
    'push'  : '',
    'pop'   : '',
    'mov'   : '\x80',
    'movsx' : '\x81',
    'movsxd': '\x82',
    'movzx' : '\x83',
    'cdqe'  : '',
    'lea'   : '',
    'add'   : '\x84',
    'sub'   : '\x85',
    'xor'   : '\x86',
    'jmp'   : '\x87',
    'je'    : '',
    'jne'   : '\x88',
    'jl'    : '',
    'jle'   : '',
    'jg'    : '',
    'jge'   : '',
    'shl'   : '\x89',
    'shr'   : '\x8a',
    'cmp'   : '',
    'test'  : '\x8b',
    'call'  : '',
    'div'   : '',
    'imul'  : ''
}

vaddr = 0

class Code:

    def __init__(self, addr, opcode, operand):

        self.addr = addr
        self.opcode = opcode
        self.operand = operand
        self.vcode = ''
        self.vaddr = 0

        self.TYPE  = []
        self.SIGN = []
        self.REF  = []
        self.SIZE = []
        self.reg  = []
        self.imm  = []

    def print_code(self):

        print '0x{:x}:\t{}\t{}'.format(self.addr, self.opcode, self.operand)
        print ''

    def virtualizatoin(self):

        '''
        Vcode Format

        [inst] [opX_meta] [opX_data]

        [inst]: 1 byte
        [opX_meta]: 1 byte [type] [sign] [ref] [size]
            [type]: 3 bit
                000 -> [imm(1)]
                001 -> [imm(3)]
                010 -> [imm(8)]
                011 -> [reg]
                100 -> [reg] [reg]
                101 -> [reg] [imm(1)]
                110 -> [reg] [imm(3)]
                111 -> [reg] [reg] [imm(1)]
            [sign]: 2 bit
            [ref]:  1 bit
            [size]: 2 bit
        [opX_data]: 1~8 byte
        '''

        # [inst]
        self.vcode += inst_table[self.opcode]

        # [opX_meta]
        ops = re.split(r', ', self.operand)
        for op in ops:

            if op == '':
                break

            print '< {} >'.format(op)

            TYPE = 0b000
            SIGN = 0b00
            REF  = 0b0
            SIZE = 0b00

            reg = []
            imm = 0

            # immediate only
            if op[0].isdigit():

                # [imm]
                imm_str = op.replace('0x', '')
                imm = int(imm_str, 16)

                # [type]
                if len(imm_str) <= 2:
                    TYPE = 0b000
                elif len(imm_str) <= 6:
                    TYPE = 0b001
                elif len(imm_str) <= 16:
                    TYPE = 0b010
                else:
                    print 'imm error'
                    sys.exit(1)

            # reference
            elif '[' in op:

                # [ref]
                REF = 0b1

                op = op.replace('[', '').replace(']', '')
                comps = re.split(r' ', op)

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

                comps = comps[2:]

                # differentiate by component length
                if len(comps) == 1:

                    # [type]
                    TYPE = 0b011

                    # check if * in component
                    if '*' in comps[0]:
                        mul = comps[0][-1:]
                        r = reg_table[comps[0][:-2]]
                        if mul == '2':
                            m = 0b01
                        elif mul == '4':
                            m = 0b10
                        elif mul == '8':
                            m = 0b11

                        # [reg]
                        reg.append(chr(ord(r) | (m << 6)))

                    else:

                        # [reg]
                        reg.append(reg_table[comps[0].decode('utf-8')])

                elif len(comps) == 3:

                    # [sign]
                    if comps[1] == '-':
                        SIGN = 0b10

                    # differentiate by third component
                    if comps[2][0].isdigit():

                        # check if * in component
                        if '*' in comps[0]:
                            mul = comps[0][-1:]
                            r = reg_table[comps[0][:-2]]
                            if mul == '2':
                                m = 0b01
                            elif mul == '4':
                                m = 0b10
                            elif mul == '8':
                                m = 0b11

                            # [reg]
                            reg.append(chr(ord(r) | (m << 6)))

                        else:

                            # [reg]
                            reg.append(reg_table[comps[0].decode('utf-8')])

                        # [imm]
                        imm_str = comps[2].replace('0x', '')
                        imm = int(imm_str, 16)

                        # [type]
                        if len(imm_str) <= 2:
                            TYPE = 0b101
                        elif len(imm_str) <= 6:
                            TYPE = 0b110
                        else:
                            print 'imm error'
                            sys.exit(1)

                    else:

                        # [type]
                        TYPE = 0b100

                        for i in range(2):
                            idx = i * 2

                            # check if * in component
                            if '*' in comps[idx]:
                                mul = comps[idx][-1:]
                                r = reg_table[comps[idx][:-2]]
                                if mul == '2':
                                    m = 0b01
                                elif mul == '4':
                                    m = 0b10
                                elif mul == '8':
                                    m = 0b11

                                # [reg]
                                reg.append(chr(ord(r) | (m << 6)))

                            else:

                                # [reg]
                                reg.append(reg_table[comps[idx].decode('utf-8')])

                elif len(comps) == 5:

                    # [type]
                    TYPE = 0b111

                    # [sign]
                    if comps[1] == '-':
                        SIGN |= 0b10
                    if comps[3] == '-':
                        SIGN |= 0b01

                    for i in range(2):
                        idx = i * 2

                        # check if * in component
                        if '*' in comps[idx]:
                            mul = comps[idx][-1:]
                            r = reg_table[comps[idx][:-2]]
                            if mul == '2':
                                m = 0b01
                            elif mul == '4':
                                m = 0b10
                            elif mul == '8':
                                m = 0b11

                            # [reg]
                            reg.append(chr(ord(r) | (m << 6)))

                        else:

                            # [reg]
                            reg.append(reg_table[comps[idx].decode('utf-8')])

                    # [imm]
                    imm_str = comps[4].replace('0x', '')
                    imm = int(imm_str, 16)

                else:
                    print 'type error'

            # register only
            else:

                # [type]
                TYPE = 0b011

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

                # [reg]
                reg.append(reg_table[op.decode('utf-8')])

            print 'TYPE:   {0:03b}'.format(TYPE)
            print 'SIGN:   {0:02b}'.format(SIGN)
            print 'REF:    {:b}'.format(REF)
            print 'SIZE:   {0:02b}'.format(SIZE)
            print 'reg:    {}'.format(reg)
            print 'imm:    0x{:x}'.format(imm)
            print ''

            self.TYPE.append(TYPE)
            self.SIGN.append(SIGN)
            self.REF.append(REF)
            self.SIZE.append(SIZE)
            self.reg.append(reg)
            self.imm.append(imm)

            # append opX_meta to vcode
            meta = chr((TYPE << 5) + (SIGN << 3) + (REF << 2) + SIZE)
            self.vcode += meta

            # append opX_data to vcode
            if TYPE == 0b000:
                self.vcode += p64(imm)[:1]
            elif TYPE == 0b001:
                self.vcode += p64(imm)[:3]
            elif TYPE == 0b010:
                self.vcode += p64(imm)
            elif TYPE == 0b011:
                self.vcode += reg[0]
            elif TYPE == 0b100:
                self.vcode += reg[0]
                self.vcode += reg[1]
            elif TYPE == 0b101:
                self.vcode += reg[0]
                self.vcode += p64(imm)[:1]
            elif TYPE == 0b110:
                self.vcode += reg[0]
                self.vcode += p64(imm)[:3]
            elif TYPE == 0b111:
                self.vcode += reg[0]
                self.vcode += reg[1]
                self.vcode += p64(imm)[:1]
            else:
                print 'type error'

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
        if codes[i].opcode.startswith('j') and codes[i].TYPE[0] <= 0b010:
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
                mod_vcode = codes[i].vcode[:1] + '\x30' + p64(offset)[:3]
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
