#include <gtest/gtest.h>

#include "word.h"


TEST(WordTest, SignedUnsignedConversion) {
    Word w;
    w.u = 3;
    ASSERT_EQ(3, w.s);

    w.u = 0xffffffff;
    ASSERT_EQ(-1, w.s);
}


TEST(Word, GetBits) {
    Word w;
    w.u = 0b11111111111111111111111111111111;
    ASSERT_EQ(0b111, (w.bits<5, 3>)());
    ASSERT_EQ(0xff, (w.bits<26, 19>)());

    w.u = 0b10101010101010101010000000000000;
    ASSERT_EQ(0b101, (w.bits<29, 27>)());
    ASSERT_EQ(0, (w.bits<12, 0>)());
}


TEST(Word, DefaultsToZero) {
    Word words[0xffff];
    for (auto w: words) {
        ASSERT_EQ(0u, w.u);
    }
}
