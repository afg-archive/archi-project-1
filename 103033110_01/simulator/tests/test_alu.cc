#include <gtest/gtest.h>

#include "alu.h"


constexpr int32_t posh = 1 << 30;
constexpr int32_t negh = -posh;


TEST(ALU, Overflow) {
    EXPECT_FALSE(wrong_sign(0, 0, 0));
    EXPECT_FALSE(wrong_sign(0, 1, 1));
    EXPECT_FALSE(wrong_sign(2, 0, 2));
    EXPECT_FALSE(wrong_sign(0, -3, -3));
    EXPECT_FALSE(wrong_sign(-4, 0, -4));
    EXPECT_FALSE(wrong_sign(5, 5, 10));
    EXPECT_FALSE(wrong_sign(-6, -6, -12));
    EXPECT_FALSE(wrong_sign(7, -7, 0));
    EXPECT_FALSE(wrong_sign(8, -9, -1));
    EXPECT_FALSE(wrong_sign(-10, 11, 1));
}


TEST(ALU, Overflow2) {
    EXPECT_TRUE(wrong_sign(posh, posh, negh + negh));
    EXPECT_FALSE(wrong_sign(negh, negh, negh + negh));
}


TEST(ALU, Overflow3) {
    EXPECT_TRUE(wrong_sign(-1, -1, 0));
    EXPECT_TRUE(wrong_sign(0, 0, -1));
}


TEST(ALU, ErrorState) {
    ErrorState es;
    ALU alu(es);

    alu.signed_add(posh, posh);

    EXPECT_TRUE(es.warnings[NumberOverflow]);
    EXPECT_FALSE(es.fatals.any());
}
