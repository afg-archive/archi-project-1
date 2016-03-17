#pragma once
#include <cstddef>
#include <exception>
#include "word.h"
#include "code.h"


using std::size_t;


class memory_misalign: public std::exception {};

class address_overflow: public std::exception {};


template <
    class WordClass,
    size_t WordSize,
    class OverflowException = address_overflow,
    class MisalignException = memory_misalign
>
class Memory {
protected:
    const size_t size;
    WordClass* wp;
public:
    // constructs memory with size in bytes
    Memory(size_t size): size(size) {
        wp = new WordClass[size];
    }

    ~Memory() {
        delete [] wp;
    }

    // get word by index
    virtual WordClass& get(size_t index) {
        if (index < size) {
            return wp[index];
        } else {
            throw OverflowException();
        }
    }

    // get word by location in bytes
    WordClass& operator[] (size_t addr) {
        if (addr % WordSize) throw MisalignException();
        return get(addr / WordSize);
    }
};


class DAddressOverflow: public address_overflow {};
class IAddressOverflow: public address_overflow {};
class RAddressOverflow: public address_overflow {};
class DMemoryMisalign: public memory_misalign {};
class IMemoryMisalign: public memory_misalign {};


using DMemory = Memory<Word, 4, DAddressOverflow, DMemoryMisalign>;
using IMemory = Memory<Code, 4, IAddressOverflow, IMemoryMisalign>;


using _RMemoryBase = Memory<Word, 1, RAddressOverflow>;

class RMemory: public _RMemoryBase {
    using _RMemoryBase::_RMemoryBase;  // inherit constructor
    Word& get(size_t index) override {
        if (not index) {
            wp[0].u = 0u;
        }
        return _RMemoryBase::get(index);
    }
};
