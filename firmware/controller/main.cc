#include <avr/pgmspace.h>
#include <stdio.h>

#include "io.hh"
#include "ram.hh"
#include "repl.hh"
#include "serial.hh"

int main()
{
    io_init();

    Serial serial = Serial::init();
    SPI spi;
    RAM ram(spi);

    Repl repl(serial, ram);

    serial.clrscr();
    while (true)
        repl.execute();
}
