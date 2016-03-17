#pragma once
#include <cstddef>
#include <exception>
#include "word.h"


using std::size_t;


class memory_misalign: public std::exception {
};

class address_overflow: public std::exception {};


class Memory {
private:
    size_t size;
    Word* wp;
public:
    Memory(size_t); // constructs memory with size in bytes
    ~Memory();
    Word& get(size_t);  // get word by index
    Word& operator[] (size_t); // get word by location in bytes
};
