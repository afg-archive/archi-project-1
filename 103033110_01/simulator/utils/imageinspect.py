#!/usr/bin/env python3

import sys
import struct
import ctypes


class BitsBase(ctypes.Union):
    _fields_ = [
        ('signed', ctypes.c_int32),
        ('u', ctypes.c_uint32)
    ]

    def __str__(self):
        return format(self.u, '032b')

    def __repr__(self):
        return '<Bits[31:0] {:032b}>'.format(self.u)

    def part(self, msb, lsb=None):
        if lsb is None:
            lsb = msb
        return (self.u >> lsb) & ((2 << (msb - lsb)) - 1)

    def __getitem__(self, item):
        if isinstance(item, int):
            return self.part(item)
        else:
            assert item.step is None
            return self.part(item.start, item.stop)

    def __eq__(self, other):
        raise TypeError("Don't compare this directly")

    __ne__ = __eq__


class Bits(BitsBase):

    def __init__(self, value=0):
        super().__init__()
        self.u = value


class Code(Bits):

    @property
    def opcode(self):
        return self[31:26]

    @property
    def s(self):
        return self[25:21]

    @property
    def t(self):
        return self[20:16]

    @property
    def d(self):
        return self[15:11]

    @property
    def c5(self):
        return self[10:6]

    @property
    def funct(self):
        return self[5:0]

    @property
    def c16u(self):
        return self[15:0]

    @property
    def c16s(self):
        return struct.unpack('h', struct.pack('H', self[15:0]))[0]

    @property
    def c26u(self):
        return self[25:0]

    def __getitem__(self, item):
        if isinstance(item, str):
            return getattr(self, item)
        else:
            return super().__getitem__(item)


class Registry:

    def __init__(self):
        self.gis = {}
        self.ris = {}

    def instruction(self, opcode=0, *, dis, funct=None):
        if opcode:
            self.gis[opcode] = dis
        else:
            self.ris[funct] = dis

    def explain(self, code):
        if code.opcode:
            try:
                dis = self.gis[code.opcode]
            except KeyError:
                return '# [31:26] Invalid opcode 0x{:x}'.format(
                    code.opcode)
        else:
            try:
                dis = self.ris[code.funct]
            except KeyError:
                return '# [ 5: 0] Invalid funct 0x{:x}'.format(code.funct)
        return ' '.join(
            s.ljust(4) for s in
            dis.format(
                d=code.d, s=code.s, t=code.t, c5=code.c5,
                c16s=code.c16s, c16u=code.c16u, c26u=code.c26u).split()
        )


registry = Registry()


i = registry.instruction


i(funct=0x20, dis='add ${d}, ${s}, ${t}')
i(funct=0x21, dis='addu ${d}, ${s}, ${t}')
i(funct=0x22, dis='sub ${d}, ${s}, ${t}')
i(funct=0x24, dis='and ${d}, ${s}, ${t}')
i(funct=0x25, dis='or ${d}, ${s}, ${t}')
i(funct=0x26, dis='xor ${d}, ${s}, ${t}')
i(funct=0x27, dis='nand ${d}, ${s}, ${t}')
i(funct=0x28, dis='nand ${d}, ${s}, ${t}')
i(funct=0x2a, dis='slt ${d}, ${s}, ${t}')
i(funct=0x00, dis='sll ${d}, ${t}, {c5}')
i(funct=0x02, dis='srl ${d}, ${t}, {c5}')
i(funct=0x03, dis='sra ${d}, ${t}, {c5}')
i(funct=0x08, dis='jr ${s}')
i(0x08, dis='addi ${t}, ${s}, {c16s}')
i(0x09, dis='addiu ${t}, ${s}, {c16u}')
i(0x23, dis='lw ${t}, {c16s}(${s})')
i(0x21, dis='lh ${t}, {c16s}(${s})')
i(0x25, dis='lhu ${t}, {c16s}(${s})')
i(0x20, dis='lb ${t}, {c16s}(${s})')
i(0x24, dis='lbu ${t}, {c16s}(${s})')
i(0x2b, dis='sw ${t}, {c16s}(${s})')
i(0x29, dis='sh ${t}, {c16s}(${s})')
i(0x28, dis='sb ${t}, {c16s}(${s})')
i(0x0f, dis='lui ${t}, {c16u}')
i(0x0c, dis='andi ${t}, ${s}, {c16u}')
i(0x0d, dis='ori ${t}, ${s}, {c16u}')
i(0x0e, dis='nori ${t}, ${s}, {c16u}')
i(0x0a, dis='slti ${t}, ${s}, {c16s}')
i(0x04, dis='beq ${s}, ${t}, {c16s}')
i(0x05, dis='bne ${s}, ${t}, {c16s}')
i(0x07, dis='bgtz ${s}, {c16s}')
i(0x02, dis='j {c26u}')
i(0x03, dis='jal {c26u}')
i(0x3f, dis='halt')


del i


def main(filename, iim=False):
    fs = '{:>11}  {:>11}  {:>11}  {:>4}  {:<34}'

    def print_head():
        print(fs.replace(':', ':=').format('', '', '', '', ''))
        print(fs.format(
            'Signed', 'Unsigned', 'Hexadecimal', 'Addr', 'Meaning'))
        print(fs.replace(':', ':=').format('', '', '', '', ''))

    print_head()

    def pfnumber(word, meaning, addr):
        print(
            fs.format(
                word.signed, word.u, ' 0x{:08x}'.format(word.u), addr, meaning)
        )

    with open(filename, mode='rb') as file:
        initial, = struct.unpack('>I', file.read(4))
        pfnumber(
            Bits(initial), 'Initialize {} to {}'.format(
                'PC' if iim else 'SP',
                initial), 'N/A')

        size, = struct.unpack('>I', file.read(4))
        pfnumber(
            Bits(size), 'Load {} Words into {} memory'.format(
                size,
                'I' if iim else 'D'
            ), 'N/A')

        i = 0
        for i in range(iim and initial // 4, (iim and initial // 4) + size):
            code = Code(struct.unpack('>I', file.read(4))[0])
            pfnumber(code, registry.explain(code), i * 4)

    if i > 20:
        print_head()


if __name__ == '__main__':
    if not (
        sys.argv[1].endswith('iimage.bin') or
        sys.argv[1].endswith('dimage.bin')
    ):
        raise SystemExit(
            'Error: filename must ends with iimage.bin or dimage.bin')
    main(sys.argv[1], sys.argv[1].endswith('iimage.bin'))
