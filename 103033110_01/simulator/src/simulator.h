#include "memory.h"
#include "code.h"

class Simulator {
public:
    Memory dmem;  // data memory
    Memory imem;  // instruction memory
    Memory rmem;  // register memory
    Simulator();
    void run();
};
