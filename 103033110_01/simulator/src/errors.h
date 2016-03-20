#pragma once

#include <bitset>


enum Warning: uint32_t {
    WriteToRegisterS0,
    NumberOverflow,
    N_WARNINGS,
};


enum Fatal: uint32_t {
    DOverflow,
    DMisalign,
    IOverflow,
    IMisalign,
    N_FATALS,
};


struct ErrorState {
    std::bitset<N_WARNINGS> warnings;
    std::bitset<N_FATALS> fatals;
    ErrorState(): warnings(0), fatals(0) {
    }
    void clear() {
        warnings.reset();
        fatals.reset();
    }
};
