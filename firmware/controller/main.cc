#include "devices/z80.hh"
#include "io.hh"
#include "devices/ram.hh"
#include "repl.hh"
#include "protocol/serial.hh"
#include "devices/sdcard.hh"
#include "fortuna1.hh"
#include "breakpoints.hh"

int main()
{
    // I/O ports
    io_init();

    // protocols
    Serial serial = Serial::init();
    SPI spi;
    
    // data structures
    Buffer      buffer { {0}, 0 };
    Breakpoints breakpoints;
    
    // devices
    RAM ram(spi);
    SDCard sdcard(spi, ram);
    Terminal terminal {};
    
    Z80 z80(buffer);
    terminal.set_z80(z80);
    
    // computer
    Fortuna1 fortuna1(ram, sdcard, z80, terminal, breakpoints);
    z80.set_fortuna1(&fortuna1);

    // communication
    Repl repl(serial, fortuna1, buffer);
    while (true)
        repl.execute();
}
