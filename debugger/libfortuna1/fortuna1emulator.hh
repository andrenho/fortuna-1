#ifndef LIBFORTUNA1_FORTUNA1EMULATOR_HH
#define LIBFORTUNA1_FORTUNA1EMULATOR_HH

#include <cstddef>
#include "fortuna1.hh"

class Fortuna1Emulator : public Fortuna1 {
public:
    size_t free_mem() const override { return 1000; }
};

#endif
