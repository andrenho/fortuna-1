#include "fortuna1emulator.hh"

#include <iostream>

void Fortuna1Emulator::test_debug_messages() const
{
    for (int i = 0; i < 3; ++i)
        std::cout << "Debug message " << i << " from emulator.\n";
}

std::string Fortuna1Emulator::test_dma() const
{
    return "Hello";
}
