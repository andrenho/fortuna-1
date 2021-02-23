#include "fortuna1realhardware.hh"

#include <iostream>

int main()
{
    Fortuna1RealHardware f("/dev/ttyUSB0");
    std::cout << f.free_mem() << "\n";
}

