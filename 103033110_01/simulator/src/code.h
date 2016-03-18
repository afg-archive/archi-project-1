#pragma once
#include "word.h"


class Code: public Word {
public:
    uint32_t opcode() const;
    uint32_t rs() const;
    uint32_t rt() const;
    uint32_t rd() const;
    uint32_t c_shamt() const;
    uint32_t funct() const;

    uint32_t c_immu() const; // immediate, unsigned
    int32_t c_imms() const;  // immediate, signed

    uint32_t c_addr() const;
};
