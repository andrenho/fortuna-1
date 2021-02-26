#include "io.hh"

#include <avr/io.h>

//
// setters
//

#define P(name, port, pin)                                 \
    static bool p_ ## name = false;                        \
    void set_ ## name(bool v)                              \
    {                                                      \
        if (v)                                             \
            PORT ## port |= _BV(PORT ## port ## pin);      \
        else                                               \
            PORT ## port &= ~_BV(PORT ## port ## pin);     \
        p_ ## name = v;                                    \
    }                                                      \

OUTPUT_PORTS
IO_PORTS
#undef P

// 
// getters
//

#define P(name, port, pin)                                 \
    bool get_ ## name()                                    \
    {                                                      \
        return PIN ## port & _BV(PIN ## port ## pin);      \
    }                                                      \
    static void set_ ## name ## _as_pullup()               \
    {                                                      \
        DDR ## port &= ~_BV(PIN ## port ## pin);           \
        PORT ## port &= ~_BV(PORT ## port ## pin);         \
    }
INPUT_PORTS
IO_PORTS
#undef P

//
// initialize ports
//

void io_init()
{
#define P(name, port, pin) DDR ## port |= (1 << PIN ## port ## pin);
    OUTPUT_PORTS
#undef P
#define P(name, port, pin) set_ ## name ## _as_pullup();
    INPUT_PORTS
    IO_PORTS
#undef P
}

// vim:ts=4:sts=4:sw=4:expandtab
