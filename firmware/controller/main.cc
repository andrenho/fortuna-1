#include <stdio.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include "devices/z80.hh"
#include "io.hh"
#include "devices/ram.hh"
#include "repl.hh"
#include "protocol/serial.hh"
#include "devices/sdcard.hh"
#include "fortuna1.hh"

int main()
{
    io_init();

    Serial serial = Serial::init();
    SPI spi;

    RAM ram(spi);
    SDCard sdcard(spi);
    Z80 z80(ram);
    Fortuna1 fortuna1(ram, sdcard, z80);

    Repl repl(serial, fortuna1);

    // printf_P(PSTR("System initialized.\n"));

    // serial.clrscr();
    while (true)
        repl.execute();
}
