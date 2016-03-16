#include <gtest/gtest.h>
#include "code.h"


TEST(Code, BitsAlias) {
    Code c;
    //          10987654321098765432109876543210
    //          ******-----*****-----*****------
    c.u =     0b10100011001110101110101101110111;
    EXPECT_EQ(0b101000,                           c.opcode());
    EXPECT_EQ(      0b11001,                      c.rs());
    EXPECT_EQ(           0b11010,                 c.rt());
    EXPECT_EQ(                0b11101,            c.rd());
    EXPECT_EQ(                     0b01101,       c.c_shamt());
    EXPECT_EQ(                          0b110111, c.funct());
    EXPECT_EQ(                0b1110101101110111, c.c_immu());
    EXPECT_EQ(      0b11001110101110101101110111, c.c_addr());
}


TEST(Code, ImmediateSigned) {
    Code c;

    c.u = 0b1111111111111111;
    EXPECT_EQ(-1, c.c_imms());

    c.u = 0b1000000000000000;
    EXPECT_EQ(-0x8000, c.c_imms());

    c.u = 0;
    EXPECT_EQ(0, c.c_imms());
}
