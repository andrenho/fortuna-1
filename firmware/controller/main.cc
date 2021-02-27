#include <stdio.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

#include "io.hh"
#include "ram.hh"
#include "repl.hh"
#include "serial.hh"

#define SPI_DDR DDRB
#define CS      PINB2
#define MOSI    PINB3
#define MISO    PINB4
#define SCK     PINB5

int main()
{
    io_init();

    Serial serial = Serial::init();
    SPI spi;

    RAM ram(spi);
    const char* s = ram.test();
    printf("%s\n", s);

    /*
    Repl repl(serial, ram);

    serial.clrscr();
    while (true)
        repl.execute();
    */
}
