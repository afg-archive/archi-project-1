#include "memory.h"
#include "code.h"

class Simulator {
public:
    DMemory dmem;  // data memory
    IMemory imem;  // instruction memory
    RMemory rmem;  // register memory
    Simulator();
    void run();
};
