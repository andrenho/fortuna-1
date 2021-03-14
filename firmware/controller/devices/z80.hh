#ifndef CONTROLLER_Z80_HH
#define CONTROLLER_Z80_HH

#include "ram.hh"

struct Z80Pins {
    // inputs
    bool int_  : 1;
    bool nmi   : 1;
    bool rst   : 1;
    bool busrq : 1;
    
    // outputs
    bool halt  : 1;
    bool iorq  : 1;
    bool m1    : 1;
    bool busak : 1;
};

class Z80 {
public:
    explicit Z80(RAM& ram);
    
    void powerdown();
    void startup();
    
    void request_bus();
    void step();
    
    bool powered() const { return power_; }
    uint32_t cycle_count() const { return cycle_count_; }
    uint16_t pc() const { return pc_; }

private:
    RAM&     ram_;
    bool     power_ = false;
    uint32_t cycle_count_ = 0;
    uint16_t pc_ = 0;
    
    void cycle();
    void check_iorq();

    Z80Pins state() const;
};

#endif
