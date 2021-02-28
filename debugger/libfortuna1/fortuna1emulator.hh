#ifndef LIBFORTUNA1_FORTUNA1EMULATOR_HH
#define LIBFORTUNA1_FORTUNA1EMULATOR_HH

#include <cstddef>
#include "fortuna1.hh"

class Fortuna1Emulator : public Fortuna1 {
public:
    size_t free_mem() const override { return 1000; }
    
    void        test_debug_messages() const override;
    std::string test_dma() const override;
    
    void                 ram_write_byte(uint16_t addr, uint8_t data) override;
    uint8_t              ram_read_byte(uint16_t addr) const override;
    void                 ram_write_buffer(uint16_t addr, std::vector<uint8_t> const& bytes) override;
    std::vector<uint8_t> ram_read_buffer(uint16_t addr, uint16_t sz) const override;

private:
    uint8_t ram_[64 * 1024] = {0};
};

#endif
