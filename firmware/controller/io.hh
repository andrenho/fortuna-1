#ifndef IO_H_
#define IO_H_

#include <stdbool.h>
#include <stdint.h>

void io_init();

// ports only written by the microcontroller (set_XXX)
#define OUTPUT_PORTS    \
    P(AVCS,   C, 4)     \
    P(DMACS,  C, 2)     \
    P(BUSREQ, B, 2)     \
    P(SDCS,   C, 1)     \
    P(ZRST,   C, 5)     \
    P(ZCLK,   D, 5)     \
    P(NMI,    C, 3)     \
    P(INT,    D, 2)

// ports only read by the microcontroller (get_XXX)
#define INPUT_PORTS     \
    P(MISO,    B, 4)     \
    P(DMA_RDY, B, 1)
    
// ports both read and written by the microcontroller (get_XXX, set_XXX)
//    (the SPI ports are here so that they can be disabled)
#define IO_PORTS        \
    P(MOSI,   B, 3)     \
    P(SCLK,   B, 5)     \
    P(BUSACK, C, 0)
    
#define P(name, port, pin) \
    void set_ ## name(uint8_t v); \
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
