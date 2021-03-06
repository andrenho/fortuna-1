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
    
    virtual ResetStatus              hard_reset() = 0;
    virtual ResetStatus              soft_reset() = 0;
    virtual void                     system_reset() = 0;
    
    virtual void                     ram_write_byte(uint16_t addr, uint8_t data) = 0;
    virtual uint8_t                  ram_read_byte(uint16_t addr) const = 0;
    virtual void                     ram_write_buffer(uint16_t addr, std::vector<uint8_t> const& bytes) = 0;
    virtual std::vector<uint8_t>     ram_read_buffer(uint16_t addr, uint16_t sz) const = 0;
    
    virtual SDCardStatus             sdcard_status() const = 0;
    virtual std::array<uint8_t, 512> sdcard_read(uint32_t block) = 0;
    
    virtual Z80_Info                 z80_info() const = 0;
    virtual Z80_Info                 z80_step() = 0;
    virtual Z80_Info                 z80_nmi() = 0;
    
    virtual void                     keypress(uint16_t key) = 0;
    
    virtual std::vector<uint16_t>    list_breakpoints() const = 0;
    virtual std::vector<uint16_t>    add_breakpoint(uint16_t address) = 0;
    virtual std::vector<uint16_t>    remove_breakpoint(uint16_t address) = 0;
    virtual std::vector<uint16_t>    remove_all_breakpoints() = 0;
    
    virtual void                     run() = 0;
    virtual void                     stop() = 0;
    virtual EventType                last_event() const = 0;
    
    virtual void set_log_bytes(bool) {}
    virtual void set_log_messages(bool) {}

protected:
    Fortuna1() = default;
};

#endif
