#ifndef CONTROLLER_FORTUNA1_HH
#define CONTROLLER_FORTUNA1_HH

#include "devices/ram.hh"
#include "devices/sdcard.hh"
#include "devices/z80.hh"

class Fortuna1 {
public:
    Fortuna1(RAM& ram, SDCard& sdcard, Z80& z80) : ram_(ram), sdcard_(sdcard), z80_(z80) {}
    
    RAM&    ram() const { return ram_; }
    SDCard& sdcard() const { return sdcard_; }
    Z80&    z80() const { return z80_; }

private:
    RAM& ram_;
    SDCard& sdcard_;
    Z80& z80_;
};

#endif
