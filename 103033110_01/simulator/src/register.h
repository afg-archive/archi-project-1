#pragma once

#include "word.h"


class RegisterSpace {
private:
    Word data[32];
public:
    RegisterSpace(): data() {}
    Word& operator[](size_t offset) {
        if (not offset) {
            data[0].u = 0;
        }
        return data[0];
    }
};
