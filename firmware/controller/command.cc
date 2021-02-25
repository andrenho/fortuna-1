#include "command.hh"

// for every new command, include:
//   - here
//   - repl_do
//   - parse_repl_request

#include <avr/pgmspace.h>

#include "serial.hh"

extern Serial serial;

int free_ram()
{
    extern int __heap_start, *__brkval;
    volatile int v;
    int free_ = (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
    return free_;
}

void test_debug_messages()
{
    for (int i = 0; i < 3; ++i)
        serial.debug_P(PSTR("Debug message %d..."), i);
}
