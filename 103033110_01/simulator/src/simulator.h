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
class FatalHalt: public std::exception {};


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
    void cycle() {
        es.clear();
        ++ cycle_count;
        execute(Code(I[pc].getu32()));
        write_snapshot();
        write_errors();
        if (es.fatals.any()) {
            loghere << "Fatal error in cycle: " << cycle_count << std::endl;
            throw FatalHalt();
        }
    }
    void run() {
        write_snapshot();
        try {
            while (true) {
                cycle();
            }
        }
        catch (const Halt&) {}
        catch (const FatalHalt&) {}
    }

    void write_snapshot() {
        dumphere << "cycle " << cycle_count << '\n';
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

    void write_error(const char* str) {
        errorhere << "In cycle " << cycle_count << ": " << str << "\n";
    }
    void write_warning_if(Warning w, const char* str) {
        if (es.warnings[w]) write_error(str);
    }
    void write_fatal_if(Fatal f, const char* str) {
        if (es.fatals[f]) write_error(str);
    }
    void write_errors() {
        write_warning_if(WriteToRegisterS0, "Write $0 Error");
        write_warning_if(NumberOverflow, "Number Overflow");
        write_fatal_if(DOverflow, "Address Overflow");
        write_fatal_if(DMisalign, "Misalignment Error");
        write_fatal_if(IOverflow, "(ext) I memory address overflow");
        write_fatal_if(IMisalign, "(ext) I memory address misalign");
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
        #define get_sc_addr() (alu.signed_add(R[i.rs()].u, i.c_imms()))
        #define branch() (pc = alu.signed_add(alu.signed_add(pc, 4), 4 * i.c_imms()))
        switch (i.opcode()) {
        case r_delegate:
            execute_r_delegate(i);
            return; // execute_r_delegate() is responsible to manipulate pc
        case addi:
            R[i.rt()].s = alu.signed_add(R[i.rs()].s, i.c_imms());
            break;
        case addiu:
            R[i.rt()].u = R[i.rs()].u + i.c_immu();
            break;
        case lw:
            R[i.rt()].u = M[get_sc_addr()].getu32();
            break;
        case lh:
            R[i.rt()].s = M[get_sc_addr()].gets16();
            break;
        case lhu:
            R[i.rt()].u = M[get_sc_addr()].getu16();
            break;
        case lb:
            R[i.rt()].s = M[get_sc_addr()].gets8();
            break;
        case lbu:
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
            R[i.rt()].u = i.c_immu() << 16u;
            break;
        case andi:
            R[i.rt()].u = R[i.rs()].u & i.c_immu();
            break;
        case ori:
            R[i.rt()].u = R[i.rs()].u | i.c_immu();
            break;
        case nori:
            R[i.rt()].u = ~(R[i.rs()].u | i.c_immu());
            break;
        case slti:
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
            // XXX $s signed?
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
