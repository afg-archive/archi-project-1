#pragma once
#include <endian.h>
#include <cstdint>
#include <cstddef>
#include <string.h>
#include "errors.h"


template <class ResultType, class InputType>
ResultType type_convert(InputType value) {
    static_assert(sizeof(ResultType) == sizeof(InputType), "ResultType must have the same size of InputType");
    union { InputType input; ResultType res; } conv {value};
    return conv.res;
}


template <
    Fatal OverflowError,
    Fatal MisalignError
>
class Memory {
    class Proxy {
    private:
        Memory& mem;
        size_t offset;
    public:
        Proxy(Memory& mem, size_t offset): mem(mem), offset(offset) {}
    private:
        void set_error(Fatal Error) {
            if (mem.esp) {
                mem.esp->fatals.set(Error);
            }
        }
        template <class T>
        bool check() {
            bool clean = true;
            if (offset + sizeof(T) > mem.Bytes) {
                clean = false;
                set_error(OverflowError);
            }
            if (offset % sizeof(T)) {
                clean = false;
                set_error(MisalignError);
            }
            return clean;
        }
        template <class T>
        void set(T value) {
            if (check<T>()) {
                ((T*)mem.data)[offset / sizeof(T)] = value;
            }
        }
        template <class T>
        T get() {
            if (check<T>()) {
                return ((T*)mem.data)[offset / sizeof(T)];
            } else {
                return 0;
            }
        }
    public:
        void setu8(uint8_t value) {
            set<uint8_t>(value);
        }
        uint8_t getu8() {
            return get<uint8_t>();
        }
        void sets8(int8_t value) {
            set<int8_t>(value);
        }
        int8_t gets8() {
            return get<int8_t>();
        }
        void setu16(uint16_t value) {
            set<uint16_t>(htobe16(value));
        }
        uint16_t getu16() {
            return be16toh(get<uint16_t>());
        }
        void sets16(int16_t value) {
            setu16(type_convert<uint16_t>(value));
        }
        int16_t gets16() {
            return type_convert<int16_t>(getu16());
        }
        void setu32(uint32_t value) {
            set<uint32_t>(htobe32(value));
        }
        uint32_t getu32() {
            return be32toh(get<uint32_t>());
        }
        void sets32(int32_t value) {
            setu32(type_convert<uint32_t>(value));
        }
        int32_t gets32() {
            return type_convert<int32_t>(getu32());
        }
    };
public:
    const size_t Bytes;
    ErrorState* esp;
    char* buffer;
    void* data;
public:
    Memory(size_t Bytes, ErrorState& es):
        Bytes(Bytes),
        esp(&es),
        buffer(new char[Bytes]()),
        data(buffer) {}
    Memory(size_t Bytes):
        Bytes(Bytes),
        esp(nullptr),
        buffer(new char[Bytes]()),
        data(buffer) {}
    ~Memory() {
        delete[] buffer;
    }
    Proxy at(size_t offset) {
        return Proxy(*this, offset);
    }
    Proxy operator[] (size_t offset) {
        return at(offset);
    }
    void clear() {
        memset(data, 0, Bytes);
    }
};


using DMemory = Memory<DOverflow, DMisalign>;
using IMemory = Memory<IOverflow, IMisalign>;
using RMemory = Memory<ROverflow, UnspecifiedMemoryError>;
