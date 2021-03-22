#include "terminal.hh"

#include "z80.hh"

void Terminal::keypress(uint8_t key)
{
    z80_->interrupt(key);
}
