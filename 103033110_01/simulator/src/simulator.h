#include <endian.h>
#include <iostream>
#include <exception>
#include <stdexcept>
#include <algorithm>
#include "memory.h"
#include "errors.h"


uint32_t load_bigendian(std::istream& is) {
    union {
        char buf[4];
        uint32_t value;
    } temp;
    if (not is.read(temp.buf, 4)) {
        throw std::runtime_error("Cannot read bytes from stream");
    };
    return be32toh(temp.value);
}


class Halt: public std::exception {};


class Simulator {
public:
    uint32_t pc;
    uint32_t cycle_count;
    IMemory* imem;
    DMemory* dmem;
    RMemory* rmem;
    std::ostream& dumphere;
    std::ostream& errorhere;
    std::ostream& loghere;
    Simulator(
        std::ostream& dumphere = std::cout,
        std::ostream& errorhere = std::cout,
        std::ostream& loghere = std::cout):
        pc(0),
        cycle_count(0),
        imem(nullptr),
        dmem(nullptr),
        rmem(new RMemory(32 * 4)),
        dumphere(dumphere),
        errorhere(errorhere),
        loghere(loghere)
    {}

    ~Simulator() {
        delete imem;
        delete dmem;
        delete rmem;
    }

    void load_pc(std::istream& is) {
        pc = load_bigendian(is);
        loghere << "PC initialized to " << pc << '\n';
    }
    void load_imem(size_t size, std::istream& is) {
        delete imem;
        imem = new IMemory(size);
        auto actual = is.read(imem->buffer, size).gcount();
        loghere << "loaded " << actual << " bytes I memory\n";
    }
    void load_iimage(std::istream& is) {
        load_pc(is);
        auto size = load_bigendian(is);
        loghere << size << " bytes to load into I memory\n";
        load_imem(size, is);
    }
    void load_sp(std::istream& is) {
        rmem->at(29 * 4).setu32(load_bigendian(is));
        loghere << "SP ($29) initialized to " << rmem->at(29 * 4).getu32() << '\n';
    }
    void load_dmem(size_t size, std::istream& is) {
        delete dmem;
        dmem = new DMemory(1024);
        auto actual = is.read(dmem->buffer, std::min(1024ul, size)).gcount();
        loghere << "loaded " << actual << " bytes D memory\n";
    }
    void load_dimage(std::istream& is) {
        load_sp(is);
        auto size = load_bigendian(is);
        loghere << size << " bytes to load into D memory\n";
        load_dmem(size, is);
    }
    void cycle() {
        ++ cycle_count;
    }
    void run() {
        while (true) {
            try {
                cycle();
            } catch (const Halt&) {}
        }
    }
};
