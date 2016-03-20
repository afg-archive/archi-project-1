#include <gtest/gtest.h>
#include <sstream>
#include "simulator.h"


using namespace std;


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

    ASSERT_EQ(0xeacd4399, sim.rmem->at(29 * 4).getu32());
}



TEST(Simulator, LoadDMemory) {
    istringstream iss(
        "\x12\x34\x56\x78"
        "\x90\xab\xcd\xef"
    );
    Simulator sim;
    sim.load_dmem(8, iss);

    EXPECT_EQ(1024, sim.dmem->Bytes);
    EXPECT_EQ(0x12345678, sim.dmem->at(0).getu32());
    EXPECT_EQ(0x90abcdef, sim.dmem->at(4).getu32());
}


TEST(Simulator, LoadIMemory) {
    istringstream iss(
        "\x90\xab\xcd\xef"
        "\x12\x34\x56\x78"
    );
    Simulator sim;
    sim.load_imem(8, iss);

    EXPECT_EQ(8, sim.imem->Bytes);
    EXPECT_EQ(0x90abcdef, sim.imem->at(0).getu32());
    EXPECT_EQ(0x12345678, sim.imem->at(4).getu32());
}


TEST(Simulator, LoadDImage) {
    istringstream iss(
        string(
            "\xea\xcd\x43\x99"
            "\x00\x00\x00\x08"
            "\x12\x34\x56\x78"
            "\x90\xab\xcd\xef",
            16
        )
    );
    Simulator sim;
    sim.load_dimage(iss);

    EXPECT_EQ(0xeacd4399, sim.rmem->at(29 * 4).getu32());
    EXPECT_EQ(0x12345678, sim.dmem->at(0).getu32());
    EXPECT_EQ(0x90abcdef, sim.dmem->at(4).getu32());
}


TEST(Simulator, LoadIImage) {
    istringstream iss(
        string(
            "\xf2\x56\x9a\xde"
            "\x00\x00\x00\x08"
            "\x90\xab\xcd\xef"
            "\x12\x34\x56\x78",
            16
        )
    );

    Simulator sim;
    sim.load_iimage(iss);

    EXPECT_EQ(0xf2569ade, sim.pc);
    EXPECT_EQ(0x90abcdef, sim.imem->at(0).getu32());
    EXPECT_EQ(0x12345678, sim.imem->at(4).getu32());
}


TEST(Simulator, RMemory) {
    Simulator sim;

    for (uint32_t i = 0; i < 32u; ++ i) {
        sim.rmem->at(i * 4).setu32(0xe9234927 + i);
        EXPECT_EQ(0xe9234927 + i, sim.rmem->at(i * 4).getu32());
    }
}
