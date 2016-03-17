#include <gtest/gtest.h>

#include "memory.h"


TEST(Memory, ZeroInit) {
    Memory m(1024);

    EXPECT_EQ(0u, m[0].u);
    EXPECT_EQ(0u, m[4092].u);
}


TEST(Memory, MemoryMisalign) {
    Memory m(1024);

    ASSERT_THROW(m[3], memory_misalign);
}


TEST(Memory, AddressOverflow) {
    Memory m(1024);

    EXPECT_THROW(m[4096], address_overflow);
    EXPECT_THROW(m.get(1024), address_overflow);
}
