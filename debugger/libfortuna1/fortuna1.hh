#ifndef LIBFORTUNA1_FORTUNA1_HH
#define LIBFORTUNA1_FORTUNA1_HH

#include <array>
#include <cstdint>
#include <string>
#include <vector>
#include "libf1comm/fields/sdcardstatus.hh"
#include "libf1comm/fields/z80info.hh"

class Fortuna1 {
public:
    virtual ~Fortuna1() = default;
    
    virtual size_t free_mem() const = 0;
    
    virtual void        test_debug_messages() const = 0;
    virtual std::string test_dma() const = 0;
    
    virtual void                     hard_reset() = 0;
    virtual void                     soft_reset() = 0;
    
    virtual void                     ram_write_byte(uint16_t addr, uint8_t data) = 0;
    virtual uint8_t                  ram_read_byte(uint16_t addr) const = 0;
    virtual void                     ram_write_buffer(uint16_t addr, std::vector<uint8_t> const& bytes) = 0;
    virtual std::vector<uint8_t>     ram_read_buffer(uint16_t addr, uint16_t sz) const = 0;
    virtual uint8_t                  data_bus() const = 0;
    virtual void                     set_data_bus(uint8_t data) = 0;
    
    virtual SDCardStatus             sdcard_status() const = 0;
    virtual std::array<uint8_t, 512> sdcard_read(uint32_t block) = 0;
    
    virtual Z80_Info                 z80_info() const = 0;
    
    virtual void set_log_bytes(bool) {}
    virtual void set_log_messages(bool) {}

protected:
    Fortuna1() = default;
};

#endif
