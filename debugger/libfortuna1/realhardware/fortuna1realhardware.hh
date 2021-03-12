#ifndef LIBFORTUNA1_FORTUNA1REALHARDWARE_HH
#define LIBFORTUNA1_FORTUNA1REALHARDWARE_HH

#include <string>
#include "../libf1comm/buffer.hh"
#include "../fortuna1.hh"
#include "serial.hh"

class Fortuna1RealHardware : public Fortuna1 {
public:
    explicit Fortuna1RealHardware(std::string const& serial_port);
    
    void set_log_bytes(bool b) override { serial_.set_log_bytes(b); }
    void set_log_messages(bool b) override { serial_.set_log_message(b); }
    
    void hard_reset() override;
    void soft_reset() override;
    
    size_t free_mem() const override;
    
    void        test_debug_messages() const override;
    std::string test_dma() const override;
    
    void                 ram_write_byte(uint16_t addr, uint8_t data) override;
    uint8_t              ram_read_byte(uint16_t addr) const override;
    void                 ram_write_buffer(uint16_t addr, std::vector<uint8_t> const& bytes) override;
    std::vector<uint8_t> ram_read_buffer(uint16_t addr, uint16_t sz) const override;
    uint8_t              data_bus() const override;
    void                 set_data_bus(uint8_t data) override;
    
    SDCardStatus             sdcard_status() const override;
    std::array<uint8_t, 512> sdcard_read(uint32_t block) override;
    
    Z80_Info z80_info() const override;

private:
    mutable Buffer buffer_ { {0}, 0 };
    Serial serial_;
};

#endif