#ifndef LIBFORTUNA1_FORTUNA1REALHARDWARE_HH
#define LIBFORTUNA1_FORTUNA1REALHARDWARE_HH

#include <string>
#include "fortuna1.hh"
#include "serial.hh"

class Fortuna1RealHardware : public Fortuna1 {
public:
    explicit Fortuna1RealHardware(std::string const& serial_port);
    ~Fortuna1RealHardware() override;
    
    void set_log_bytes(bool b) override { serial_.set_log_bytes(b); }
    void set_log_messages(bool b) override { serial_.set_log_message(b); }
    
    size_t free_mem() const override;

private:
    Serial serial_;
};

#endif