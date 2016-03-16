#include "word.h"


uint32_t // returns word[msb:lsb]
Word::bits(
    uint32_t msb,  // most significant bit
    uint32_t lsb   // least significant bit
) {
    return (this->u >> lsb) & ((2 << (msb - lsb)) - 1);
}
