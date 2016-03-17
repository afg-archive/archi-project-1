#include <gtest/gtest.h>

#include "memory.h"
#include "word.h"


TEST(Memory, ZeroInit) {
    Memory<Word, 4> m(1024);

    EXPECT_EQ(0u, m[0].u);
    EXPECT_EQ(0u, m[4092].u);
}


TEST(Memory, MemoryMisalign) {
    Memory<Word, 4> m(1024);

    ASSERT_THROW(m[3], memory_misalign);
}


TEST(Memory, AddressOverflow) {
    Memory<Word, 4> m(1024);

    EXPECT_THROW(m[4096], address_overflow);
    EXPECT_THROW(m.get(1024), address_overflow);
}


TEST(Memory, Writable) {
    Memory<Word, 4> m(1024);

    m[4].u = 3;
    ASSERT_EQ(3, m[4].u);
    ASSERT_EQ(3, m[4].s);
    ASSERT_EQ(3, m.get(1).u);
    ASSERT_EQ(3, m.get(1).s);
}


TEST(RMemory, ZeroReadOnly) {
    RMemory r(1024);

    r[0].u = 1u;
    ASSERT_EQ(0u, r[0].u);

    r[0].s = -53423;
    ASSERT_EQ(0u, r[0].u);

    r[0].u = 0xff;
    ASSERT_EQ(0u, r[0].u);
}
