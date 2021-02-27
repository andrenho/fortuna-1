#ifndef IO_H_
#define IO_H_

#include <stdbool.h>

void io_init();

// ports only written by the microcontroller (set_XXX)
#define OUTPUT_PORTS    \
    P(AVCS,   C, 4)     \
    P(DMACS,  C, 2)     \
    P(BUSREQ, B, 2)     \
    P(SDCS,   C, 1)

// ports only read by the microcontroller (get_XXX)
#define INPUT_PORTS     \
    P(MISO,   B, 4)

// ports both read and written by the microcontroller (get_XXX, set_XXX)
#define IO_PORTS        \
    P(MOSI,   B, 3)     \
    P(SCLK,   B, 5)     \

#define P(name, port, pin) \
    void set_ ## name(bool v); \
    void set_ ## name ## _as_output();
OUTPUT_PORTS
IO_PORTS
#undef P

#define P(name, port, pin) \
    bool get_ ## name(); \
    void set_ ## name ## _as_input(); \
    void set_ ## name ## _as_pullup();
INPUT_PORTS
IO_PORTS
#undef P

#endif

// vim:ts=4:sts=4:sw=4:expandtab
