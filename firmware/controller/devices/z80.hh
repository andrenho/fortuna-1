#ifndef CONTROLLER_Z80_HH
#define CONTROLLER_Z80_HH

#include "ram.hh"

class Z80 {
public:
    explicit Z80(RAM& ram) : ram_(ram) {}
    
    void powerdown();
    void startup();

private:
    RAM& ram_;
};

#endif
