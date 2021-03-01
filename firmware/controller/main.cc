#include <stdio.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

#include "io.hh"
#include "ram.hh"
#include "repl.hh"
#include "serial.hh"

extern "C" void __cxa_pure_virtual() { while (1); }

int main()
{
    io_init();

    Serial serial = Serial::init();
    SPI spi;

    RAM ram(spi);

    Repl repl(serial, ram);

    // serial.clrscr();
    while (true)
        repl.execute();
}
