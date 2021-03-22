#ifndef CONTROLLER_TERMINAL_HH
#define CONTROLLER_TERMINAL_HH

#include <stdint.h>
#include "z80.hh"

class Terminal {
public:
    uint8_t last_printed_char() const { return last_printed_char_; }
    void set_last_printed_char(uint8_t last_printed_char) { last_printed_char_ = last_printed_char; }
    
    void set_z80(Z80& z80) { z80_ = &z80; }
    
    void keypress(uint8_t key);

private:
    uint8_t last_printed_char_ = 0;
    uint8_t last_keypress_ = 0;
    Z80*    z80_ = nullptr;
};

#endif
