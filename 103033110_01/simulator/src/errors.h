#pragma once
#include <string.h>


enum Warning {
    WriteToRegisterS0 = 1 << 0,
    NumberOverflow = 1 << 1,
};


enum FatalError {
    DOverflow = 1 << 0,
    DMisalign = 1 << 1,
    IOverflow = 1 << 2,
    IMisalign = 1 << 3,
    ROverflow = 1 << 4,
    RMisalign = 1 << 5,
};


struct ErrorState {
    Warning warning;
    FatalError fatal;
    ErrorState(): warning(0), fatal(0) {
    }
    void clear() {
        warning = 0;
        fatal = 0;
    }
};
