from random import randrange


def big_signed():
    return randrange(2 ** 30 + 1, 2 ** 31)


def big_unsigned():
    return randrange(2 ** 31 + 1, 2 ** 32)


def any_unsigned():
    return randrange(2 ** 32)


initpc = 136
set_pc(136)
set_sp(any_unsigned())


# absolute address jumps
j((4 + initpc) // 4)   # 0
sll(0, 0, 0)  # 4
j((20 + initpc) // 4)  # 8
halt()  # 12
halt()  # 16
jal((28 + initpc) // 4)  # 20
halt()  # 24
u2(initpc + 40)  # 28
jr(u2)  # 32

# sll should not emit Write $0 Error
sll(0, 0, 0)  # 36
sll(0, 0, 0)
sll(0, 0, 0)
sll(0, 0, 0)
sll(0, 0, 0)
sll(0, 0, 0)

# add overflow
add(s0, s1(big_signed()), s2(big_signed()))
add(s0, s1(-big_signed()), s2(-big_signed()))
add(s1, s2(-2 ** 31), s0(-0b11))
add(0, s2, s0)

# addu no overflow
addu(u0, u1(0xefefefef), u2(0xabababab))
addu(0, u1, u2)

# sub overflow
sub(s0, 0, s2(-0x80000000))
sub(s1, s0(big_signed()), s2(-big_signed()))
sub(s1, s0(-big_signed()), s2(big_signed()))
sub(0, s0, s2)

and_(u0, u1(any_unsigned()), u2(any_unsigned()))
or_(u0, u2, u1)
xor(u0, u1, u2)
nor(u0, u2, u1)
nand(u0, u1, u2)
and_(0, u1, u2)
or_(0, u2, u1)
xor(0, u1, u2)
nor(0, u2, u1)
nand(0, u1, u2)


slt(0, 0, 0)
slt(0, 0, s0(-1))
slt(0, 0, s0(1))
slt(0, s0(-1), 0)
slt(0, s0(1), 0)


sll(u1, u0(0b101), 0b11111)
sll(0, u0, 0b1101)
srl(u1, u0(0xadef3923), 0b10011)
srl(0, u0, 0b1001)
sra(u1, u0(0xadef3923), 0b10010)
sra(0, u1, 0b1001)


addi(14, s0(2 ** 31 - 5999), 5999)
addi(15, s0(2 ** 31 - 4999), 5999)
addi(16, s1(-2 ** 31), -1)
addi(17, s0(-2 ** 31 + 4000), -4000)
addi(18, s0(-2 ** 31 + 4000), -4010)
addi(29, u0(any_unsigned()), randrange(0x8000))
addi(0, s0, -4010)
addi(0, s1, -1)


addiu(s0, u0(any_unsigned()), randrange(0xffff))
addiu(s0, u0(any_unsigned()), randrange(0xffff))
addiu(s0, u0(any_unsigned()), randrange(0xffff))
addiu(s0, u0(1), randrange(0xffff))
addiu(s0, 0, 0xffff)  # bug?
addiu(0, u0(any_unsigned()), randrange(0xffff))
addiu(0, u0(any_unsigned()), randrange(0xffff))
addiu(0, u0(any_unsigned()), randrange(0xffff))


# memory loading

# general
lw(u0, 94, s2(-6))
lw(u0, 15, s2(1))
lh(s1, 492, s2(-4))
lhu(s2, 699, s2(-15))
lb(s1, 384, s2(137))
lbu(21, 940, s2(-23))
# write to 0
lw(0, 94, s2(-6))
lw(0, 15, s2(1))
lh(0, 492, s2(-4))
lhu(0, 699, s2(-15))
lb(0, 384, s2(137))
lbu(0, 940, s2(-23))
# no overflow / misalign here
lh(u0, 22, s1(1000))
lh(u0, 21, s1(1001))
lhu(u0, 22, s1(1000))
lhu(u0, 21, s1(1001))
lb(u0, 15, s1(256))
lb(u0, 439, s1(1023 - 439))
lbu(u0, 832, s1(1023 - 832))
lbu(u0, 18, s1(254))
# negative C
lw(u0, -4329, s1(1020 + 4329))
lw(u0, -31848, s1(1020 + 31848))
lh(u0, -8492, s1(1022 + 8492))
lhu(u0, -29481, s1(1022 + 29481))
lb(u0, -3923, s1(1023 + 3923))
lbu(u0, -9453, s1(1023 + 9453))
# negative reg value
lw(u0, 1020 + 9814, s1(-9814))
lh(u0, 1022 + 28313, s1(-28313))
lhu(u0, 1022 + 13892, s1(-13892))
lb(u0, 1023 + 5893, s1(-5893))
lbu(u0, 1023 + 13239, s1(-13239))

# store

lui(u0, 0xf)
lui(u0, 0x3492)
lui(0, 0x394)

for i in range(3):
    andi(u1, u0(any_unsigned()), randrange(0xffff))
    ori(u1, u0, randrange(0xffff))
    nori(u1, u0, randrange(0xffff))
    slti(u1, u0, randrange(-0x8000, 8000))

andi(0, u0(any_unsigned()), randrange(0xffff))
ori(0, u0, randrange(0xffff))
nori(0, u0, randrange(0xffff))
slti(0, u0, randrange(-0x8000, 0x8000))

# nop branches
beq(u0(2059), u1(0xab939293), -3)
beq(u0(320932059), u2(0x3f239293), 2933)
bne(u0(193932), u0, -19033)
bne(u0(0xf9049f92), u0, 0x39f1)
bgtz(s0(0), -4839)
bgtz(0, -493)
bgtz(s0(-4193), 29481)
bgtz(s0(0), 492)

# real branches
beq(u0(23), u1(23), 1)
j(0x876543)
bne(u0(0xffff), u1(0xff0f), 1)
j(0x123456)
bgtz(s0(1), 1)
halt()

# storing
sw(s0(20), 16, s2(-16))
sh(s0, 18, s2)
sb(s0, 19, s2)
lw(s1, 0, 0)
lw(s1, 4, 0)

# all errors with one instruction
lw(0, 7, s0(2 ** 31 - 1))

finish()
