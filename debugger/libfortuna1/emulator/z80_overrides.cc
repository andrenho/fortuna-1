#include "z80/Z80.h"

#define DEVICE_TERMINAL   0x1

#include "fortuna1emulator.hh"

extern Fortuna1Emulator* emulator;

word LoopZ80(Z80 *R)
{
    (void) R;
    emulator->increment_cycle_counter();
    return INT_QUIT;
}

void WrZ80(word Addr, byte Value)
{
    emulator->ram_write_byte(Addr, Value);
}

byte RdZ80(word Addr)
{
    return emulator->ram_read_byte(Addr);
}

void OutZ80(word Port, byte Value)
{
    switch (Port & 0xff) {
        case DEVICE_TERMINAL:
            emulator->set_last_printed_char(Value);
            break;
    }
}

byte InZ80(word Port)
{
    (void) Port;
    return 0;
}

void PatchZ80(Z80 *R)
{
    (void) R;
}
