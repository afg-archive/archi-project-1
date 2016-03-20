#pragma once

#include <cstdint>


struct Word {
    union {
        uint32_t u;  // signed value
        int32_t s;   // unsigned value
    };

    Word(uint32_t initial = 0u): u(initial) {}

    // word[msb:lsb] note: b = bit
    template <uint32_t msb, uint32_t lsb>
    uint32_t bits() const {
        static_assert(msb >= lsb, "msb should be greater or equal to lsb");
        return (this->u >> lsb) & ((2 << (msb - lsb)) - 1);
    }
};
