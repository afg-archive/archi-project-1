#pragma once
#include <endian.h>
#include <cstdint>
#include <cstddef>


template <class ResultType, class InputType>
ResultType type_convert(InputType value) {
    static_assert(sizeof(ResultType) == sizeof(InputType), "ResultType must have the same size of InputType");
    union { InputType input; ResultType res; } conv {value};
    return conv.res;
}


class Memory {
    class Proxy {
    private:
        Memory& mem;
        size_t offset;
    public:
        Proxy(Memory& mem, size_t offset): mem(mem), offset(offset) {}
        template <class T>
        bool check() const {
            bool clean = true;
            if (offset + sizeof(T) > mem.Bytes) {
                clean = false;
            }
            if (offset % sizeof(T)) {
                clean = false;
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
        T get() const {
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
        uint8_t getu8() const {
            return get<uint8_t>();
        }
        void sets8(int8_t value) {
            set<int8_t>(value);
        }
        int8_t gets8() const {
            return get<int8_t>();
        }
        void setu16(uint16_t value) {
            set<uint16_t>(htobe16(value));
        }
        uint16_t getu16() const {
            return be16toh(get<uint16_t>());
        }
        void sets16(int16_t value) {
            setu16(type_convert<uint16_t>(value));
        }
        int16_t gets16() const {
            return type_convert<int16_t>(getu16());
        }
        void setu32(uint32_t value) {
            set<uint32_t>(htobe32(value));
        }
        uint32_t getu32() const {
            return be32toh(get<uint32_t>());
        }
        void sets32(int32_t value) {
            setu32(type_convert<uint32_t>(value));
        }
        int32_t gets32() const {
            return type_convert<int32_t>(getu32());
        }
    };
public:
    const size_t Bytes;
    void* data;
public:
    Memory(size_t Bytes): Bytes(Bytes), data(new uint8_t[Bytes]()) {}
    ~Memory() {
        delete[] (uint8_t*)data;
    }
    Proxy at(size_t offset) {
        return Proxy(*this, offset);
    }
    Proxy operator[] (size_t offset) {
        return at(offset);
    }
};
