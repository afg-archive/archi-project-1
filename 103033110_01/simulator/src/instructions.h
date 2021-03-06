#pragma once


enum r_instruction {
    add  = 0x20,
    addu = 0x21,
    sub  = 0x22,
    and_ = 0x24,
    or_  = 0x25,
    xor_ = 0x26,
    nor  = 0x27,
    nand = 0x28,
    slt  = 0x2a,
    sll  = 0x00,
    srl  = 0x02,
    sra  = 0x03,
    jr   = 0x08,
};

enum {
    r_delegate = 0,
    addi  = 0x08,
    addiu = 0x09,
    lw    = 0x23,
    lh    = 0x21,
    lhu   = 0x25,
    lb    = 0x20,
    lbu   = 0x24,
    sw    = 0x2b,
    sh    = 0x29,
    sb    = 0x28,
    lui   = 0x0f,
    andi  = 0x0c,
    ori   = 0x0d,
    nori  = 0x0e,
    slti  = 0x0a,
    beq   = 0x04,
    bne   = 0x05,
    bgtz  = 0x07,
    j     = 0x02,
    jal   = 0x03,
    halt  = 0x3f,
};
