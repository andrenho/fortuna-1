#include "z80.hh"

#include "io.hh"

Z80::Z80(RAM& ram)
        : ram_(ram)
{
    powerdown();
}

void Z80::powerdown()
{
    set_Z80RST(0);
}

void Z80::startup()
{

}

void Z80::request_bus()
{

}

