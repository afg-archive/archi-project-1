#include "memory.h"


Memory::Memory(size_t size): size(size) {
    wp = new Word[size];
}


Memory::~Memory() {
    delete [] wp;
}


Word& Memory::operator[] (size_t addr) {
    if (addr % 4) throw memory_misalign();
    if (addr / 4 < size) {
        return wp[addr / 4];
    } else {
        throw address_overflow();
    }
}
