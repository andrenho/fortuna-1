#ifndef CONTROLLER_Z80_HH
#define CONTROLLER_Z80_HH

#include "ram.hh"
#include "terminal.hh"

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
    using EachCycle = void(*)(bool first, void* data);
    
    Z80(RAM& ram, Terminal& terminal);
    
    void powerdown();
    void startup();
    
    void request_bus(EachCycle f_each_cycle = nullptr, void* data = nullptr);
    void step(EachCycle f_each_cycle = nullptr, void* data = nullptr);
    void cycle(bool check_iorq = false, EachCycle f_each_cycle = nullptr, void* data = nullptr);
    
    bool powered() const { return power_; }
    uint32_t cycle_count() const { return cycle_count_; }
    uint16_t pc() const { return pc_; }
    
    Z80Pins state() const;

private:
    RAM&      ram_;
    Terminal& terminal_;
    bool      power_ = false;
    uint32_t  cycle_count_ = 0;
    uint16_t  pc_ = 0;
    
    void check_iorq(EachCycle f_each_cycle = nullptr, void* data = nullptr);
    
    void out(uint16_t addr, uint8_t data);
    
    uint8_t in(uint16_t addr);
};

#endif
