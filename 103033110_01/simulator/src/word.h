#pragma once

#include <cstdint>


struct Word {
    union {
        uint32_t u;  // signed value
        int32_t s;   // unsigned value
    };

    Word(uint32_t initial = 0u): u(initial) {}

    // word[m:l]
    uint32_t bits(uint32_t, uint32_t) const {
        return (this->u >> l) & ((2 << (m - l)) - 1);
    }
};
