#include "memory.h"


Memory::Memory(size_t size): size(size) {
    wp = new Word[size];
}


Memory::~Memory() {
    delete [] wp;
}


Word& Memory::get(size_t index) {
    if (index < size) {
        return wp[index];
    } else {
        throw address_overflow();
    }
}


Word& Memory::operator[] (size_t addr) {
    if (addr % 4) throw memory_misalign();
    return get(addr / 4);
}
