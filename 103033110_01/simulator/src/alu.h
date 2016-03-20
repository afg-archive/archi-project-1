#pragma once
#include <cstdint>
#include "errors.h"


inline bool wrong_sign(int32_t x, int32_t y, int32_t z) {
    // by definition from spec:
    // An addition overflow occurs if two same sign addends produce a sum of different sign.
    return ((x < 0) == (y < 0)) and ((x < 0) xor (z < 0));
}


class ALU {
    ErrorState& es;
public:
    ALU(ErrorState& es): es(es) {}
    int32_t signed_add(int32_t x, int32_t y) {
        int32_t z = x + y;
        if (wrong_sign(x, y, z)) {
            es.warnings[NumberOverflow];
        }
        return z;
    }
};
