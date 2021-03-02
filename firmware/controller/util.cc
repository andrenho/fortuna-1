#include "util.hh"

#include <avr/pgmspace.h>

int free_ram()
{
    extern int __heap_start, *__brkval;
    volatile int v;
    int free_ = (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
    return free_;
}