#pragma once

#include <algorithm>
#include <endian.h>
#include <exception>
#include <iomanip>
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
    IMemory I;
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
        I(1024, es),
        M(1024, es),
        R(),
        dumphere(dumphere),
        errorhere(errorhere),
        loghere(loghere)
    {}

    void load_pc(std::istream& is) {
        pc = load_bigendian(is);
        loghere << "PC initialized to " << pc << '\n';
    }
    void load_imem(size_t location, size_t size, std::istream& is) {
        if (location % 4ul) {
            throw std::runtime_error("PC location is not a multiple of 4, cannot load memory!");
        }
        I.clear();
        auto actual = is.read(I.buffer + location, std::min(1024ul, size)).gcount();
        loghere << "loaded " << actual << " bytes I memory\n";
    }
    void load_iimage(std::istream& is) {
        load_pc(is);
        auto nwords = load_bigendian(is);
        loghere << nwords << " words to load into I memory\n";
        load_imem(pc, nwords * 4u, is);
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
        auto nwords = load_bigendian(is);
        loghere << nwords << " words to load into D memory\n";
        load_dmem(nwords * 4u, is);
    }
    bool cycle() {
        es.clear();
        ++ cycle_count;
        try {
            execute(Code(I[pc].getu32()));
        } catch (const Halt&) {
            return false;
        }
        write_errors();
        if (es.fatals.any()) {
            loghere << "Fatal error in cycle: " << std::setbase(10) << cycle_count << std::endl;
            return false;
        }
        write_snapshot();
        return true;
    }
    void run() {
        write_snapshot();
        while (cycle());
    }

    void write_snapshot() {
        dumphere << "cycle " << std::setbase(10) << cycle_count << '\n';
        for (size_t i = 0ul; i < 32ul; ++ i) {
            dumphere << '$'
                     << std::setbase(10) << std::setfill('0') << std::setw(2) << i
                     << ": 0x"
                     << std::setbase(16) << std::setw(8) << std::uppercase << R[i].u
                     << '\n';
        }
        dumphere << "PC: 0x"
                 << std::setbase(16) << std::setw(8) << std::uppercase << pc << "\n\n\n";
    }

    void write_error(const char* str, std::ostream& to) {
        to << "In cycle " << std::setbase(10) << cycle_count << ": " << str << "\n";
    }
    void write_warning_if(Warning w, const char* str, std::ostream& to) {
        if (es.warnings[w]) write_error(str, to);
    }
    void write_fatal_if(Fatal f, const char* str, std::ostream& to) {
        if (es.fatals[f]) write_error(str, to);
    }
    void write_errors() {
        write_warning_if(WriteToRegisterS0, "Write $0 Error", errorhere);
        write_warning_if(NumberOverflow, "Number Overflow", errorhere);
        write_fatal_if(DOverflow, "Address Overflow", errorhere);
        write_fatal_if(DMisalign, "Misalignment Error", errorhere);
        write_fatal_if(IOverflow, "(ext) I memory address overflow", loghere);
        write_fatal_if(IMisalign, "(ext) I memory address misalign", loghere);
    }

    void check_r0(size_t offset) {
        if (not offset) {
            es.warnings[WriteToRegisterS0] = true;
        }
    }

    // execute

    void execute(Code i) {
        // A note on return vs break:
        // if the instruction modifies PC, use return
        // so pc += 4 won't be executed
        // otherwise, use break.
        #define get_sc_addr() (alu.signed_add(R[i.rs()].u, i.c_imms()))
        #define branch() (pc = alu.signed_add(alu.signed_add(pc, 4), 4 * i.c_imms()))
        switch (i.opcode()) {
        case r_delegate:
            execute_r_delegate(i);
            return; // execute_r_delegate() is responsible to manipulate pc
        case addi:
            check_r0(i.rt());
            R[i.rt()].s = alu.signed_add(R[i.rs()].s, i.c_imms());
            break;
        case addiu:
            check_r0(i.rt());
            R[i.rt()].s = R[i.rs()].s + i.c_imms();   // XXX wait for response
            break;
        case lw:
            check_r0(i.rt());
            R[i.rt()].u = M[get_sc_addr()].getu32();
            break;
        case lh:
            check_r0(i.rt());
            R[i.rt()].s = M[get_sc_addr()].gets16();
            break;
        case lhu:
            check_r0(i.rt());
            R[i.rt()].u = M[get_sc_addr()].getu16();
            break;
        case lb:
            check_r0(i.rt());
            R[i.rt()].s = M[get_sc_addr()].gets8();
            break;
        case lbu:
            check_r0(i.rt());
            R[i.rt()].u = M[get_sc_addr()].getu8();
            break;
        case sw:
            M[get_sc_addr()].setu32(R[i.rt()].u);
            break;
        case sh:
            M[get_sc_addr()].setu16(R[i.rt()].u & 0xffff);
            break;
        case sb:
            M[get_sc_addr()].setu8(R[i.rt()].u & 0xff);
            break;
        case lui:
            check_r0(i.rt());
            R[i.rt()].u = i.c_immu() << 16u;
            break;
        case andi:
            check_r0(i.rt());
            R[i.rt()].u = R[i.rs()].u & i.c_immu();
            break;
        case ori:
            check_r0(i.rt());
            R[i.rt()].u = R[i.rs()].u | i.c_immu();
            break;
        case nori:
            check_r0(i.rt());
            R[i.rt()].u = ~(R[i.rs()].u | i.c_immu());
            break;
        case slti:
            check_r0(i.rt());
            R[i.rt()].u = R[i.rs()].s < i.c_imms();
            break;
        case beq:
            if (R[i.rs()].u == R[i.rt()].u) {
                branch();
                return;
            }
            break;
        case bne:
            if (R[i.rs()].u != R[i.rt()].u) {
                branch();
                return;
            }
            break;
        case bgtz:
            if (R[i.rs()].s > 0) {
                branch();
                return;
            }
            break;
        case j:
            pc = Word(pc + 4u).bits<31, 28>() | 4u * i.c_addr();
            return;
        case jal:
            R[31] = pc + 4u;
            pc = Word(pc + 4u).bits<31, 28>() | 4u * i.c_addr();
            return;
        case halt:
            throw Halt();
        }
        #undef get_sc_addr
        #undef branch
        pc += 4;
    }

    void execute_r_delegate(Code i) {
        // check r0 if no break
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
            if (i.rd() == 0 and i.rt() == 0 and i.c_shamt() == 0) { // NOP
                pc += 4;
                return;
            }
            R[i.rd()].u = R[i.rt()].u << i.c_shamt();
            break;
        case srl:
            R[i.rd()].u = R[i.rt()].u >> i.c_shamt();
            break;
        case sra:
            R[i.rd()].s = R[i.rt()].s >> i.c_shamt();
            break;
        case jr:
            pc = R[i.rs()].u;
            return;
        }
        check_r0(i.rd());
        pc += 4;
    }
};
