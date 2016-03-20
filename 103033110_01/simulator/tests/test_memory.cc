#include <gtest/gtest.h>
#include "memory.h"


TEST(Memory2, ByteOrder) {
    DMemory m(4);
    m[0].setu8(0x12);
    m[1].setu8(0x34);
    m[2].setu8(0x56);
    m[3].setu8(0x78);

    ASSERT_EQ(0x12, m[0].getu8());
    ASSERT_EQ(0x34, m[1].getu8());
    ASSERT_EQ(0x56, m[2].getu8());
    ASSERT_EQ(0x78, m[3].getu8());

    ASSERT_EQ(0x1234, m[0].getu16());
    ASSERT_EQ(0x5678, m[2].getu16());

    ASSERT_EQ(0x12345678, m[0].getu32());
}


TEST(Memory2, GCCWTF) {
    DMemory m(4);

    m[0].setu16(0x1234);
    ASSERT_EQ(0x12340000, m[0].getu32());

    m[0].setu16(0x5678);
    ASSERT_EQ(0x56780000, m[0].getu32());
}


TEST(Memory2, SetUnsigned) {
    DMemory m(4);

    m[0].setu32(0x98765432);

    ASSERT_EQ(0x98765432, m[0].getu32());

    m[0].setu16(0x3322);
    m[2].setu16(0xabcd);

    ASSERT_EQ(0x3322abcd, m[0].getu32());

    m[0].setu8(0xa1);
    m[1].setu8(0xb2);
    m[2].setu8(0xc3);
    m[3].setu8(0xd4);

    ASSERT_EQ(0xa1b2c3d4, m[0].getu32());
}


TEST(Memory2, SetSigned) {
    DMemory m(4);

    m[0].sets32(-1);
}


TEST(Memory2, SignedUnsigned) {
    DMemory m(4);

    m[0].sets8(-1);

    ASSERT_EQ(0xff, m[0].getu8());
    ASSERT_EQ(0xff00, m[0].getu16());
    ASSERT_EQ(0xff000000, m[0].getu32());
}


TEST(Memory2, SignedUnsigned2) {
    DMemory m(4);

    m[0].sets32(-1);

    ASSERT_EQ(-1, m[0].gets8());
    ASSERT_EQ(-1, m[1].gets8());
    ASSERT_EQ(-1, m[2].gets8());
    ASSERT_EQ(-1, m[3].gets8());

    ASSERT_EQ(-1, m[0].gets16());
    ASSERT_EQ(-1, m[2].gets16());
}



TEST(Memory2, MisalignGuard) {
    DMemory m(1024);

    m[0].sets32(-1);

    ASSERT_EQ(-1, m[0].gets32());

    m[1].sets32(0);

    ASSERT_EQ(-1, m[0].gets32());
}


TEST(Memory2, MisalignError) {
    ErrorState es;
    DMemory m(1024, es);

    m[1].getu32();

    EXPECT_FALSE(es.warnings.any());
    ASSERT_TRUE(es.fatals[DMisalign]);

    es.fatals[DMisalign] = false;
    ASSERT_FALSE(es.fatals.any());
}


TEST(Memory2, OverflowError) {
    ErrorState es;
    DMemory m(1024, es);

    m[2000].getu32();

    EXPECT_FALSE(es.warnings.any());
    ASSERT_TRUE(es.fatals[DOverflow]);

    es.fatals[DOverflow] = false;
    ASSERT_FALSE(es.fatals.any());

}


TEST(Memory2, MisalignAndOverflow) {
    ErrorState es;
    DMemory m(1024, es);

    m[1021].getu32();

    EXPECT_FALSE(es.warnings.any());
    ASSERT_TRUE(es.fatals[DOverflow]);
    ASSERT_TRUE(es.fatals[DMisalign]);

    es.fatals[DOverflow] = false;
    es.fatals[DMisalign] = false;
    ASSERT_FALSE(es.fatals.any());
}
