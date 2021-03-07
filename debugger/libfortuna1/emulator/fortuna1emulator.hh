#ifndef LIBFORTUNA1_FORTUNA1EMULATOR_HH
#define LIBFORTUNA1_FORTUNA1EMULATOR_HH

#include <cstddef>
#include "../fortuna1.hh"
#include "sdcard_emulated.hh"

class Fortuna1Emulator : public Fortuna1 {
public:
    size_t free_mem() const override { return 1000; }
    
    void        test_debug_messages() const override;
    std::string test_dma() const override;
    
    void                 ram_write_byte(uint16_t addr, uint8_t data) override;
    uint8_t              ram_read_byte(uint16_t addr) const override;
    void                 ram_write_buffer(uint16_t addr, std::vector<uint8_t> const& bytes) override;
    std::vector<uint8_t> ram_read_buffer(uint16_t addr, uint16_t sz) const override;
    uint8_t              data_bus() const override { return 0; }
    void                 set_data_bus(uint8_t) override {}
    
    SDCardStatus sdcard_status() const override;
    

private:
    uint8_t        ram_[64 * 1024] = {0};
    SDCardEmulated sd_card_ {};
};

#endif
