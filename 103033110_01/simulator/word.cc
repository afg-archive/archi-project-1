#include "word.h"


uint32_t // returns word[msb:lsb]
Word::bits(
    uint32_t msb,  // most significant bit
    uint32_t lsb   // least significant bit
) {
    return (this->u >> lsb) & ((2 << (msb - lsb)) - 1);
}


#define alias(name, msb, lsb)   uint32_t Word::name() { return bits(msb, lsb); }

alias(opcode, 31, 26);
alias(rs, 25, 21);
alias(rt, 20, 16);
alias(rd, 15, 11);
alias(c_shamt, 10, 6);
alias(funct, 5, 0);
alias(c_immu, 15, 0);
alias(c_addr, 25, 0);

#undef alias


int32_t Word::c_imms() {
    union {
        int16_t s16;
        uint16_t u16;
    };
    u16 = c_immu();
    return s16;
}
