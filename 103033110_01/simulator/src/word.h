#pragma once

#include <cstdint>


struct Word {
    union {
        uint32_t u = 0u;  // signed value
        int32_t s;   // unsigned value
    };

    uint32_t bits(uint32_t, uint32_t) const;
    // word[a:b]
};
