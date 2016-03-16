#include <gtest/gtest.h>

#include "word.h"


TEST(WordTest, SignedUnsignedConversion) {
    Word w;
    w.u = 3;
    ASSERT_EQ(3, w.s);

    w.u = 0xffffffff;
    ASSERT_EQ(-1, w.s);
}
