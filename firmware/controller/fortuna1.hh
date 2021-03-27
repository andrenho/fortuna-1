#ifndef CONTROLLER_FORTUNA1_HH
#define CONTROLLER_FORTUNA1_HH

#include "devices/terminal.hh"
#include "devices/ram.hh"
#include "devices/sdcard.hh"
#include "devices/z80.hh"
#include "libf1comm/defines.hh"
#include "breakpoints.hh"

class Fortuna1 {
public:
    Fortuna1(RAM& ram, SDCard& sdcard, Z80& z80, Terminal& terminal, Breakpoints& breakpoints)
        : ram_(ram), sdcard_(sdcard), terminal_(terminal), z80_(z80), breakpoints_(breakpoints) {}
    
    uint8_t write_byte(uint16_t addr, uint8_t data);
    uint8_t read_byte(uint16_t addr) const;
    
    bool read_block(uint16_t addr, uint16_t sz, RAM::ReadFunc read_func, void* data = nullptr) const;
    bool write_block(uint16_t addr, uint16_t sz, RAM::WriteFunc write_func, void* data = nullptr);
    
    RAM&         ram() const         { return ram_; }
    SDCard&      sdcard() const      { return sdcard_; }
    Terminal&    terminal() const    { return terminal_; }
    Z80&         z80() const         { return z80_; }
    Breakpoints& breakpoints() const { return breakpoints_; }
    
    ResetStatus hard_reset(Buffer& buffer);
    ResetStatus soft_reset();
    void        system_reset();

private:
    RAM&         ram_;
    SDCard&      sdcard_;
    Terminal&    terminal_;
    Z80&         z80_;
    Breakpoints& breakpoints_;
};

#endif
