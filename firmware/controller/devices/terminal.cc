#include "terminal.hh"

void Terminal::keypress(uint8_t key)
{
    z80_->interrupt(key);
}
