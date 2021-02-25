#include "command.hh"

// for every new command, include:
//   - here
//   - repl_do
//   - parse_repl_request

#include <avr/pgmspace.h>

int Command::free_ram() const
{
    extern int __heap_start, *__brkval;
    volatile int v;
    int free_ = (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
    return free_;
}

void Command::test_debug_messages() const
{
    for (int i = 0; i < 3; ++i)
        serial_.debug_P(PSTR("Debug message %d..."), i);
}

const char* Command::test_dma() const
{
    return ram_.test();
}
