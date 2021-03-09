#ifndef CONTROLLER_Z80_HH
#define CONTROLLER_Z80_HH

#include "ram.hh"

class Z80 {
public:
    explicit Z80(RAM& ram);
    
    void powerdown();
    void startup();
    
    void request_bus();
    
    uint32_t cycle_count() const { return cycle_count_; }

private:
    RAM& ram_;
    bool power_ = false;
    uint32_t cycle_count_ = 0;
    
    void cycle();
    void check_iorq();
};

#endif
