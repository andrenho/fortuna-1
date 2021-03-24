#include "terminal.hh"

#include "z80.hh"

void Terminal::keypress(uint8_t key)
{
    last_keypress_ = key;
    z80_->interrupt(0xcf);   // RST 08h
}
