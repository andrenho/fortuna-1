#ifndef LIBFORTUNA1_FORTUNA1REALHARDWARE_HH
#define LIBFORTUNA1_FORTUNA1REALHARDWARE_HH

#include <string>
#include "fortuna1.hh"

class Fortuna1RealHardware : public Fortuna1 {
public:
    explicit Fortuna1RealHardware(std::string const& serial_port);
    
    size_t free_mem() const override;
};

#endif