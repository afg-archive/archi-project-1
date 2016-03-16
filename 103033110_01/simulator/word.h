#pragma once

#include <cstdint>


struct Word {
    union {
        uint32_t u;  // signed value
        int32_t s;   // unsigned value
    };

    uint32_t bits(uint32_t, uint32_t);
    // word[a:b]

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
