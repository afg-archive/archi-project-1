#pragma once
#include <endian.h>
#include <cstdint>
#include <cstddef>


template <class ResultType, class InputType>
ResultType type_convert(InputType value) {
    return (union { InputType input; ResultType res; }) {value} .res;
}


template <size_t Bytes>
class Memory {
    class Proxy {
    private:
        Memory& mem;
        size_t offset;
    public:
        Proxy(Memory& mem, size_t offset): mem(mem), offset(offset) {}
        template <class T>
        bool check() const {
            return true;
            // bool clean = true;
            // if (offset + sizeof(T) > Bytes) {
            //     clean = false;
            // }
            // if (offset % sizeof(T)) {
            //     clean = false;
            // }
            // return clean;
        }
        template <class T>
        void set(T value) {
            if (check<T>()) {
                for (size_t i = 0ul; i < sizeof(T); ++ i) {
                    mem.data[offset + i] = value;
                    value >>= 8;
                }
            }
        }
        template <class T>
        T get() const {
            if (check<T>()) {
                T result(0);
                for (size_t i = 0ul; i < sizeof(T); ++ i) {
                    result |= mem.data[offset + i] << (i * 8);
                }
                return result;
                // return *(T*)((void*)(mem.data + offset));
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
protected:
    uint8_t data[Bytes];
public:
    Memory(): data() {}
    Proxy operator[] (size_t offset) {
        return Proxy(*this, offset);
    }
};
