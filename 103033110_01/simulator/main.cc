#include <iostream>
#include <fstream>
#include "src/simulator.h"


using namespace std;


int main() {
    ifstream dimage("dimage.bin", ios::binary);
    ifstream iimage("iimage.bin", ios::binary);
    ofstream errordump("error_dump.rpt");
    ofstream regdump("snapshot.rpt");
    Simulator sim(regdump, errordump);
    sim.load_iimage(iimage);
    sim.load_dimage(dimage);
    iimage.close();
    dimage.close();
    sim.run();
    errordump.close();
    regdump.close();
}
