#pragma once

#include "word.h"


class Code: public Word {
public:

    using Word::Word;

    #define alias(name, msb, lsb)   uint32_t name() const { return bits<msb, lsb>(); }

    alias(opcode, 31, 26);
    alias(rs, 25, 21);
    alias(rt, 20, 16);
    alias(rd, 15, 11);
    alias(c_shamt, 10, 6);
    alias(funct, 5, 0);
    alias(c_immu, 15, 0);
    alias(c_addr, 25, 0);

    #undef alias

    // immediate, signed
    int32_t c_imms() const {
        union {
            int16_t s16;
            uint16_t u16;
        };
        u16 = c_immu();
        return s16;
    }
};
