#include <gtest/gtest.h>
#include <sstream>
#include "simulator.h"


using namespace std;


TEST(Simulator, LoadBigEndian) {
    istringstream iss("\x39\x23\x3a\x49");

    ASSERT_EQ(0x39233a49, load_bigendian(iss));
}


TEST(Simulator, LoadPC) {
    istringstream iss("\xf2\x56\x9a\xde");
    Simulator sim;
    sim.load_pc(iss);

    ASSERT_EQ(0xf2569ade, sim.pc);
}


TEST(Simulator, LoadSP) {
    istringstream iss("\xea\xcd\x43\x99");
    Simulator sim;
    sim.load_sp(iss);

    ASSERT_EQ(0xeacd4399, sim.R[29].u);
}



TEST(Simulator, LoadDMemory) {
    istringstream iss(
        "\x12\x34\x56\x78"
        "\x90\xab\xcd\xef"
    );
    Simulator sim;
    sim.load_dmem(8, iss);

    EXPECT_EQ(1024, sim.M.Bytes);
    EXPECT_EQ(0x12345678, sim.M[0].getu32());
    EXPECT_EQ(0x90abcdef, sim.M[4].getu32());
}


TEST(Simulator, LoadIMemory) {
    istringstream iss(
        "\x90\xab\xcd\xef"
        "\x12\x34\x56\x78"
    );
    Simulator sim;
    sim.load_imem(0, 8, iss);

    EXPECT_EQ(1024, sim.I.Bytes);
    EXPECT_EQ(0x90abcdef, sim.I.at(0).getu32());
    EXPECT_EQ(0x12345678, sim.I.at(4).getu32());
}


TEST(Simulator, LoadDImage) {
    istringstream iss(
        string(
            "\xea\xcd\x43\x99"
            "\x00\x00\x00\x02"
            "\x12\x34\x56\x78"
            "\x90\xab\xcd\xef",
            16
        )
    );
    Simulator sim;
    sim.load_dimage(iss);

    EXPECT_EQ(0xeacd4399, sim.R[29].u);
    EXPECT_EQ(0x12345678, sim.M[0].getu32());
    EXPECT_EQ(0x90abcdef, sim.M[4].getu32());
}


TEST(Simulator, LoadIImage) {
    istringstream iss(
        string(
            "\x00\x00\x00\xf0"
            "\x00\x00\x00\x02"
            "\x90\xab\xcd\xef"
            "\x12\x34\x56\x78",
            16
        )
    );

    Simulator sim;
    sim.load_iimage(iss);

    EXPECT_EQ(0xf0, sim.pc);
    EXPECT_EQ(0x90abcdef, sim.I.at(0 + 0xf0).getu32());
    EXPECT_EQ(0x12345678, sim.I.at(4 + 0xf0).getu32());
}


TEST(Simulator, RMemory) {
    Simulator sim;

    for (uint32_t i = 0; i < 32u; ++ i) {
        sim.R[i].u = 0xe9234927 + i;
        EXPECT_EQ(i ? 0xe9234927 + i : 0, sim.R[i].u);
    }
}


TEST(Simulator, ErrorState) {
    Simulator sim;

    istringstream iss(
        string(
            "\x00\x00\x01\x24"
            "\x00\x00\x00\x02"
            "\x90\xab\xcd\xef"
            "\x12\x34\x56\x78",
            16
        )
    );

    sim.load_iimage(iss);

    sim.M[2].getu32();
    EXPECT_TRUE(sim.es.fatals[DMisalign]);
    sim.M[1024].getu32();
    EXPECT_TRUE(sim.es.fatals[DOverflow]);

    sim.I.at(2).getu32();
    EXPECT_TRUE(sim.es.fatals[IMisalign]);
    sim.I.at(1024).getu32();
    EXPECT_TRUE(sim.es.fatals[IOverflow]);
}
