#pragma once

#include <algorithm>
#include <endian.h>
#include <exception>
#include <iostream>
#include <stdexcept>

#include "alu.h"
#include "code.h"
#include "errors.h"
#include "instructions.h"
#include "memory.h"
#include "register.h"


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
    ErrorState es;
    ALU alu;
    IMemory* imem;
    DMemory M;
    RegisterSpace R;
    std::ostream& dumphere;
    std::ostream& errorhere;
    std::ostream& loghere;
    Simulator(
        std::ostream& dumphere = std::cout,
        std::ostream& errorhere = std::cout,
        std::ostream& loghere = std::cout):
        pc(0),
        cycle_count(0),
        es(),
        alu(es),
        imem(nullptr),
        M(1024, es),
        dumphere(dumphere),
        errorhere(errorhere),
        loghere(loghere)
    {}

    ~Simulator() {
        delete imem;
    }

    void load_pc(std::istream& is) {
        pc = load_bigendian(is);
        loghere << "PC initialized to " << pc << '\n';
    }
    void load_imem(size_t size, std::istream& is) {
        delete imem;
        imem = new IMemory(size, es);
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
        R[29].u = load_bigendian(is);
        loghere << "SP ($29) initialized to " << R[29].u << '\n';
    }
    void load_dmem(size_t size, std::istream& is) {
        M.clear();
        auto actual = is.read(M.buffer, std::min(1024ul, size)).gcount();
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
        // A note on return vs break:
        // if the instruction modifies PC, use return
        // so pc += 4 won't be executed
        // otherwise, use break.
        switch (i.opcode()) {
        case r_delegate:
            execute_r_delegate(i);
            return; // execute_r_delegate() is responsible to manipulate pc
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
            R[i.rd()].s = alu.signed_add(R[i.rs()].s, R[i.rt()].s);
            break;
        case addu:
            R[i.rd()].u = R[i.rs()].u + R[i.rt()].u;
            break;
        case sub:
            R[i.rd()].s = alu.signed_add(R[i.rs()].s, -R[i.rt()].s);
            break;
        case and_:
            R[i.rd()].u = R[i.rs()].u & R[i.rt()].u;
            break;
        case or_:
            R[i.rd()].u = R[i.rs()].u | R[i.rt()].u;
            break;
        case xor_:
            R[i.rd()].u = R[i.rs()].u ^ R[i.rt()].u;
            break;
        case nor:
            R[i.rd()].u = ~(R[i.rs()].u | R[i.rt()].u);
            break;
        case nand:
            R[i.rd()].u = ~(R[i.rs()].u & R[i.rt()].u);
            break;
        case slt:
            R[i.rd()].u = R[i.rs()].s < R[i.rt()].s;
            break;
        case sll:
            R[i.rd()].u = R[i.rt()].u << i.c_shamt();
            break;
        case srl:
            R[i.rd()].u = R[i.rt()].u >> i.c_shamt();
            break;
        case sra:
            R[i.rd()].s = R[i.rt()].s >> i.c_shamt();
            break;
        case jr:
            pc = i.rs();
            return;
        }
        pc += 4;
    }
};
