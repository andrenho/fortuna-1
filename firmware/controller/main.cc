#include "io.hh"
#include "ram.hh"
#include "repl.hh"
#include "serial.hh"
#include "sdcard.hh"

int main()
{
    io_init();

    Serial serial = Serial::init();
    SPI spi;

    RAM ram(spi);
    SDCard sdcard(spi);

    Repl repl(serial, ram, sdcard);

    // serial.clrscr();
    while (true)
        repl.execute();
}
