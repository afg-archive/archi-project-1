#include <gtest/gtest.h>

#include "register.h"


TEST(Register, ZeroReadOnly) {
    RegisterSpace r;

    r[0].u = 100;

    EXPECT_EQ(0, r[0].u);
    EXPECT_EQ(0, r[0].s);
}


TEST(Register, Writable) {
    RegisterSpace r;

    for (uint32_t i = 0; i < 32u; ++ i) {
        r[i].u = 0x3958af10 + i;
    }

    EXPECT_EQ(0, r[0].u);

    for (uint32_t i = 1; i < 32u; ++ i) {
        EXPECT_EQ(0x3958af10 + i, r[i].u);
    }
}
