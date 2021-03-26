#include <algorithm>
#include "z80/Z80.h"

#include "fortuna1emulator.hh"
#include "../libf1comm/fortuna1def.hh"

extern Fortuna1Emulator* emulator;

word LoopZ80(Z80 *R)
{
    (void) R;
    emulator->increment_cycle_counter();
    
    if (emulator->nmi()) {
        emulator->set_nmi(false);
        return INT_NMI;
    }
    
    if (R->IFF && emulator->interrupt()) {
        emulator->set_interrupt(false);
        return 0xcf;  // rst 0x8
    }
    
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
        case TERMINAL:
            emulator->set_last_printed_char(Value);
            break;
        case SD_CARD: {
                auto sd = emulator->ram_read_buffer(SD_BLOCK, 6);
                uint32_t block_addr = sd.at(0) | ((uint32_t) sd.at(1) << 8) | ((uint32_t) sd.at(2) << 16) | ((uint32_t) sd.at(3) << 24);
                uint16_t ram_addr = sd.at(4) | (uint16_t) sd.at(5) << 8;
                if (Value == SD_READ) {
                    auto block = emulator->sdcard_read(block_addr);
                    std::vector<uint8_t> data_v(block.begin(), block.end());
                    emulator->ram_write_buffer(ram_addr, data_v);
                } else if (Value == SD_WRITE) {
                    auto block = emulator->ram_read_buffer(ram_addr, 512);
                    std::array<uint8_t, 512> data_a {};
                    std::copy_n(block.begin(), 512, data_a.begin());
                    emulator->sdcard_write(block_addr, data_a);
                } else {
                    emulator->ram_write_byte(SD_CARD, 0b1);
                }
            }
            break;
    }
}

byte InZ80(word Port)
{
    if ((Port & 0xff) == TERMINAL)  // keyboard
        return emulator->last_keypress();
    return 0;
}

void PatchZ80(Z80 *R)
{
    (void) R;
}
