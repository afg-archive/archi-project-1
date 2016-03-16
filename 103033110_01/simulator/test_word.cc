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
    ASSERT_EQ(0b111, w.bits(5, 3));
    ASSERT_EQ(0xff, w.bits(26, 19));

    w.u = 0b10101010101010101010000000000000;
    ASSERT_EQ(0b101, w.bits(29, 27));
    ASSERT_EQ(0, w.bits(12, 0));
}


TEST(Word, BitsAlias) {
    Word w;
    //          10987654321098765432109876543210
    //          ******-----*****-----*****------
    w.u =     0b10100011001110101110101101110111;
    EXPECT_EQ(0b101000,                           w.opcode());
    EXPECT_EQ(      0b11001,                      w.rs());
    EXPECT_EQ(           0b11010,                 w.rt());
    EXPECT_EQ(                0b11101,            w.rd());
    EXPECT_EQ(                     0b01101,       w.c_shamt());
    EXPECT_EQ(                          0b110111, w.funct());
    EXPECT_EQ(                0b1110101101110111, w.c_immu());
    EXPECT_EQ(      0b11001110101110101101110111, w.c_addr());
}


TEST(Word, ImmediateSigned) {
    Word w;

    w.u = 0b1111111111111111;
    EXPECT_EQ(-1, w.c_imms());

    w.u = 0b1000000000000000;
    EXPECT_EQ(-0x8000, w.c_imms());

    w.u = 0;
    EXPECT_EQ(0, w.c_imms());
}


TEST(Word, DefaultsToZero) {
    Word words[0xffff];
    for (auto w: words) {
        ASSERT_EQ(0u, w.u);
    }
}
