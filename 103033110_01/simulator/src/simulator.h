#include <endian.h>
#include <iostream>
#include <exception>
#include <stdexcept>
#include <algorithm>
#include "memory.h"
#include "errors.h"
#include "code.h"
#include "instructions.h"


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

    // execute

    void execute(Code i) {
        if (not i.u) {
            // NOP
            pc += 4;
            return;
        }
        switch (i.opcode()) {
        case r_delegate:
            execute_r_delegate(i);
            break;
        case addi:
            break;
        case addiu:
            break;
        case lw:
            break;
        case lh:
            break;
        case lhu:
            break;
        case lb:
            break;
        case lbu:
            break;
        case sw:
            break;
        case sh:
            break;
        case sb:
            break;
        case lui:
            break;
        case andi:
            break;
        case ori:
            break;
        case nori:
            break;
        case slti:
            break;
        case beq:
            break;
        case bne:
            break;
        case bgtz:
            break;
        case j:
            break;
        case jal:
            break;
        case halt:
            break;
        }
    }

    void execute_r_delegate(Code i) {
        switch (i.funct()) {
        case add:
            break;
        case addu:
            break;
        case sub:
            break;
        case and_:
            break;
        case or_:
            break;
        case xor_:
            break;
        case nor:
            break;
        case nand:
            break;
        case slt:
            break;
        case sll:
            break;
        case srl:
            break;
        case sra:
            break;
        case jr:
            break;
        }
    }
};
