#ifndef CONTROLLER_Z80_HH
#define CONTROLLER_Z80_HH

#include "ram.hh"
#include "terminal.hh"
#include "sdcard.hh"

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
    Z80(Buffer& buffer);
    
    bool powered() const { return power_; }
    uint32_t cycle_count() const { return cycle_count_; }
    uint16_t pc() const { return pc_; }
    
    bool debug_mode() const { return debug_mode_; }
    void set_debug_mode(bool debug_mode) { debug_mode_ = debug_mode; }
    
    void powerdown();
    void startup();
    
    void request_bus();
    void step();
    void cycle(bool check_iorq = false);
    
    Z80Pins state() const;
    void    print_pin_state() const;
    
    void interrupt(uint8_t int_value);
    void nmi();
    
    void set_fortuna1(class Fortuna1* fortuna1) { fortuna1_ = fortuna1; }
    
private:
    Buffer&   buffer_;
    Fortuna1* fortuna1_ = nullptr;
    bool      power_ = false;
    uint32_t  cycle_count_ = 0;
    uint16_t  pc_ = 0;
    int       next_interrupt_data_ = -1;
    bool      debug_mode_ = false;
    
    void    check_iorq();
    
    void    out(uint16_t addr, uint8_t value);
    uint8_t in(uint16_t addr);
};

#endif
