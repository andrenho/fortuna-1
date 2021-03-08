#ifndef CONTROLLER_Z80_HH
#define CONTROLLER_Z80_HH

#include "ram.hh"

class Z80 {
public:
    Z80(RAM& ram) : ram_(ram) {}

private:
    RAM& ram_;
};

#endif
