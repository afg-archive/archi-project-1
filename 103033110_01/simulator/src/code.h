#pragma once
#include "word.h"


class Code: public Word {
public:
    uint32_t opcode();
    uint32_t rs();
    uint32_t rt();
    uint32_t rd();
    uint32_t c_shamt();
    uint32_t funct();

    uint32_t c_immu(); // immediate, unsigned
    int32_t c_imms();  // immediate, signed

    uint32_t c_addr();
};
