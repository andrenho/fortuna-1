#include "breakpoints.hh"

bool Breakpoints::add(uint16_t addr)
{
    for (uint16_t& r_addr : list_) {
        if (r_addr == 0) {
            r_addr = addr;
            return true;
        }
    }
    return false;
}

void Breakpoints::remove(uint16_t addr)
{
    for (uint16_t& r_addr : list_)
        if (r_addr == addr)
            r_addr = 0;
}

void Breakpoints::remove_all()
{
    for (uint16_t& r_addr : list_)
        r_addr = 0;
}

bool Breakpoints::contains(uint16_t addr) const
{
    for (uint16_t const& r_addr : list_)
        if (r_addr == addr)
            return true;
    return false;
}
