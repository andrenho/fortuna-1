#ifndef CONTROLLER_Z80_HH
#define CONTROLLER_Z80_HH

#include "ram.hh"

class Z80 {
public:
    explicit Z80(RAM& ram);
    
    void powerdown();
    void startup();
    
    void request_bus();

private:
    RAM& ram_;
    bool power_ = false;
    
    void cycle();
    void check_iorq();
};

#endif
