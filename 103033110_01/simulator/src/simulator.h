#include "memory.h"
#include "code.h"

class Simulator {
    Memory dmem;  // data memory
    Memory imem;  // instruction memory
    Memory rmem;  // register memory
public:
    Simulator();
    void run();
};
