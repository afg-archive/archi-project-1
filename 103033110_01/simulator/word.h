#pragma once

#include <cstdint>


struct Word {
    union {
        uint32_t u;  // signed value
        int32_t s;   // unsigned value
    };
};
