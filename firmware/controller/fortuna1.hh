#ifndef CONTROLLER_FORTUNA1_HH
#define CONTROLLER_FORTUNA1_HH

#include "devices/ram.hh"
#include "devices/sdcard.hh"
#include "devices/z80.hh"

enum class ResetResult {
    Ok, SDCardInitializationError, SDCardReadError, DiskNotBootable,
};

class Fortuna1 {
public:
    Fortuna1(RAM& ram, SDCard& sdcard, Z80& z80) : ram_(ram), sdcard_(sdcard), z80_(z80) {}
    
    uint8_t write_byte(uint16_t addr, uint8_t data);
    uint8_t read_byte(uint16_t addr) const;
    
    bool read_block(uint16_t addr, uint16_t sz, RAM::ReadFunc read_func, void* data = nullptr) const;
    bool write_block(uint16_t addr, uint16_t sz, RAM::WriteFunc write_func, void* data = nullptr);
    
    RAM&    ram() const { return ram_; }
    SDCard& sdcard() const { return sdcard_; }
    Z80&    z80() const { return z80_; }
    
    ResetResult hard_reset(Buffer& buffer);
    ResetResult soft_reset();
    void        system_reset();

private:
    RAM& ram_;
    SDCard& sdcard_;
    Z80& z80_;
};

#endif
