#ifndef LIBFORTUNA1_FORTUNA1EMULATOR_HH
#define LIBFORTUNA1_FORTUNA1EMULATOR_HH

#include <cstddef>
#include <optional>
#include <fstream>
#include <unordered_set>
#include "../fortuna1.hh"
#include "sdcard_emulated.hh"
#include "z80/Z80.h"

class Fortuna1Emulator : public Fortuna1 {
public:
    Fortuna1Emulator();
    
    size_t free_mem() const override { return 1000; }
    
    void        test_debug_messages() const override;
    std::string test_dma() const override;
    
    ResetStatus hard_reset() override;
    ResetStatus soft_reset() override;
    void        system_reset() override;
    
    void                 ram_write_byte(uint16_t addr, uint8_t data) override;
    uint8_t              ram_read_byte(uint16_t addr) const override;
    void                 ram_write_buffer(uint16_t addr, std::vector<uint8_t> const& bytes) override;
    std::vector<uint8_t> ram_read_buffer(uint16_t addr, uint16_t sz) const override;
    
    void                     sdcard_initialize();
    void                     sdcard_set_image(std::string const& filename);
    SDCardStatus             sdcard_status() const override;
    std::array<uint8_t, 512> sdcard_read(uint32_t block) override;
    void                     sdcard_write(uint32_t block, std::array<uint8_t, 512> const& data);
    
    void keypress(uint16_t key) override;
    
    Z80_Info z80_info() const override;
    Z80_Info z80_step() override;
    Z80_Info z80_nmi() override;
    
    void increment_cycle_counter() { ++cycle_count_; }
    void set_last_printed_char(uint8_t c) { last_printed_char_ = c; }
    
    uint8_t last_keypress() const { return last_keypress_; }
    
    bool interrupt() const { return interrupt_; }
    void set_interrupt(bool interrupt) { interrupt_ = interrupt; }
    
    bool nmi() const { return nmi_; }
    void set_nmi(bool b) { nmi_ = b; }
    
    std::vector<uint16_t> list_breakpoints() const override;
    std::vector<uint16_t> add_breakpoint(uint16_t address) override;
    std::vector<uint16_t> remove_breakpoint(uint16_t address) override;
    std::vector<uint16_t> remove_all_breakpoints() override;
    
    void run() override;
    void stop() override;
    EventType last_event() const override;

private:
    uint8_t                      ram_[64 * 1024] = {0};
    SDCardEmulated               sd_card_ {};
    std::optional<std::fstream>  sd_image_stream_ {};
    Z80                          z80_ {};
    uint32_t                     cycle_count_ = 0;
    uint8_t                      last_printed_char_ = 0;
    uint8_t                      last_keypress_ = 0;
    bool                         interrupt_ = false;
    bool                         nmi_ = false;
    std::unordered_set<uint16_t> breakpoints_ {};
    
    void basic_reset();
};

#endif
