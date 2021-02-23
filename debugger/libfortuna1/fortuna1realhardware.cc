#include "fortuna1realhardware.hh"

Fortuna1RealHardware::Fortuna1RealHardware(std::string const& serial_port)
    : serial_(serial_port.c_str())
{

}

size_t Fortuna1RealHardware::free_mem() const
{
    // TODO
    return 0;
}
