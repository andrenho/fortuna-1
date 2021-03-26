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
    
    Buffer    buffer { {0}, 0 };
    
    RAM ram(spi);
    SDCard sdcard(spi, ram);
    Terminal terminal {};
    Z80 z80(ram, terminal, sdcard, buffer);
    terminal.set_z80(z80);
    Fortuna1 fortuna1(ram, sdcard, z80, terminal);

    Repl repl(serial, fortuna1, buffer);

    // printf_P(PSTR("System initialized.\n"));

    // serial.clrscr();
    while (true)
        repl.execute();
}
