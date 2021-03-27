#include "z80.hh"

#include <avr/pgmspace.h>
#include <stdio.h>

#include "io.hh"
#include "ram.hh"

#include "libf1comm/fortuna1def.hh"
#include "fortuna1.hh"

Z80::Z80(Buffer& buffer)
        : buffer_(buffer)
{
    set_BUSREQ(1);
    set_NMI(1);
    set_INT(1);
    set_ZCLK(0);
    powerdown();
}

void Z80::powerdown()
{
    set_ZRST(0);
    power_ = false;
}

void Z80::startup()
{
    set_BUSREQ(1);
    set_NMI(1);
    set_INT(1);
    
    set_ZRST(0);
    
    bool stored = debug_mode_;
    debug_mode_ = false;
    for (int i = 0; i < 50; ++i)
        cycle();
    debug_mode_ = stored;
    
    set_ZRST(1);
    power_ = true;
    cycle_count_ = 0;
    pc_ = 0;
    
    step();
}

void Z80::request_bus()
{
    if (!power_ || get_BUSACK() == 0)
        return;
    
    bool busack = 1;
    
    set_BUSREQ(0);
    while (busack == 1) {
        cycle(true);
        busack = get_BUSACK();
    }
    set_BUSREQ(1);
}

void Z80::cycle(bool check_iorq)
{
    set_ZCLK(1);
    set_ZCLK(0);
    if (debug_mode_)
        print_pin_state();
    if (check_iorq)
        this->check_iorq();
    ++cycle_count_;
}

Z80Pins Z80::state() const
{
    return (Z80Pins) {
        get_last_INT(),
        get_last_NMI(),
        get_last_ZRST(),
        get_last_BUSREQ(),
        get_HALT(),
        get_IORQ(),
        get_M1(),
        get_BUSACK(),
    };
}

void Z80::check_iorq()
{
    if (get_IORQ() == 0) {
        uint16_t addr = fortuna1_->ram().addr_bus();
        auto m = fortuna1_->ram().memory_bus();
        if (m.we == 0) {  // OUT
            uint8_t ldata = fortuna1_->ram().data_bus();
            if (debug_mode_)
                printf_P(PSTR("Writing to port 0x%04X (data = %02X).\n"), addr, ldata);
            out(addr, ldata);
        } else if (m.rd == 0) {  // IN
            uint8_t data = in(addr);
            if (debug_mode_)
                printf_P(PSTR("Reading from port 0x%04X (data = %02X).\n"), addr, data);
            fortuna1_->ram().set_data_bus(data);
            cycle();
            fortuna1_->ram().release_bus();
        } else {  // INTERRUPT
            if (debug_mode_)
                printf_P(PSTR("Interrupt! Data = 0x%02X\n"), next_interrupt_data_);
            if (next_interrupt_data_ != -1) {
                fortuna1_->ram().set_data_bus(next_interrupt_data_ & 0xff);
                cycle();
                fortuna1_->ram().release_bus();
                next_interrupt_data_ = -1;
                set_INT(1);
            }
        }
        
        while (get_IORQ() == 0)
            cycle(false);
    }
}

void Z80::step()
{
    fortuna1_->terminal().set_last_printed_char(0);
    
    bool m1 = 1;
    
again:
    uint8_t next_instruction = fortuna1_->ram().read_byte(pc_);
    bool is_extended = (next_instruction == 0xcb || next_instruction == 0xdd || next_instruction == 0xed || next_instruction == 0xfd || next_instruction == 0xfb);
    
    while (m1 == 1) {
        cycle(true);
        m1 = get_M1();
    }
    pc_ = fortuna1_->ram().addr_bus();
    
    while (m1 == 0) {
        cycle(true);
        m1 = get_M1();
    }
    
    if (is_extended)
        goto again;

//    if (mode == M_CONTINUE && (last_printed_char != 0 || bkp_hit))
//        return 0;  // we wait until the last event is consumed by the client
//
//    bool m1 = 1;
//
//    uint8_t next_instruction;
//    memory_read_page(pc, &next_instruction, 1);
//    bool is_extended = (next_instruction == 0xcb || next_instruction == 0xdd || next_instruction == 0xed || next_instruction == 0xfd);
//
//    // run cycle until M1
//    set_BUSREQ(1);
//    for (int i = 0; i < (is_extended ? 2 : 1); ++i) {
//        m1 = 1;
//        while (m1 == 1) {
//            z80_clock();
//            z80_check_iorq();
//            m1 = get_M1();
//        }
//        if (is_extended)
//            z80_clock();
//    }
//    pc = memory_read_addr();
//
//    switch (mode) {
//        case M_DEBUG:
//            // run cycle until BUSACK
//            if (mode == M_DEBUG)
//                z80_busreq();
//            break;
//        case M_CONTINUE:
//            // find out if breakpoint was hit
//            if (bkp_in_list(pc)) {
//                bkp_hit = true;
//                mode = M_DEBUG;
//                z80_busreq();
//            }
//            return 0;
//    }
//
//    uint8_t c = last_printed_char;
//    last_printed_char = 0;
//    return c;
}

void Z80::out(uint16_t addr, uint8_t value)
{
    if (debug_mode_)
        printf_P(PSTR("Z80 OUT: port 0x%02X  value 0x%02X\n"), addr & 0xff, value);
    switch (addr & 0xff) {
        case TERMINAL:     // video OUT (print char)
            fortuna1_->terminal().set_last_printed_char(value);
            break;
        case SD_CARD:
            {
                uint8_t sd[6];
                if (!fortuna1_->read_block(SD_BLOCK, 6, [](uint16_t idx, uint8_t data, void* sd) { ((uint8_t *) sd)[idx] = data; }, sd)) {
                    fortuna1_->write_byte(SD_STATUS, 0b1);
                    return;
                }
                uint32_t block_addr = sd[0] | ((uint32_t) sd[1] << 8) | ((uint32_t) sd[2] << 16) | ((uint32_t) sd[3] << 24);
                uint16_t ram_addr = sd[4] | (uint16_t) sd[5] << 8;
                if (value == SD_READ) {
                    if (debug_mode_)
                        printf_P("Reading from SDCard block 0x%X into RAM position 0x%04X.\n", block_addr, ram_addr);
                    if (!fortuna1_->sdcard().read_page(block_addr, buffer_))
                        goto read_error;
                    if (!fortuna1_->write_block(ram_addr, 512, [](uint16_t idx, void* pdata) { return ((uint8_t *)pdata)[idx]; }, buffer_.data))
                        goto read_error;
                } else if (value == SD_WRITE) {
                    if (debug_mode_)
                        printf_P("Writing to SDCard block 0x%X from RAM position 0x%04X.\n", block_addr, ram_addr);
                    if (!fortuna1_->read_block(ram_addr, 512, [](uint16_t idx, uint8_t data, void* b) { ((uint8_t*) b)[idx] = data; }, buffer_.data))
                        goto write_error;
                    if (!fortuna1_->sdcard().write_page(block_addr, buffer_))
                        goto write_error;
                } else {
                    fortuna1_->write_byte(SD_STATUS, 0b1);
                    return;
                }
                return;
write_error:
                fortuna1_->write_byte(SD_STATUS, 0b1001);
                return;
read_error:
                fortuna1_->write_byte(SD_STATUS, 0b101);
                return;
            }
            break;
    }
}

uint8_t Z80::in(uint16_t addr)
{
    if (debug_mode_)
        printf_P(PSTR("Z80 IN: port 0x%02X\n"), addr & 0xff);
    if ((addr & 0xff) == TERMINAL) {     // keyboard IN (last key pressed)
        return fortuna1_->terminal().last_keypress();
    }
    return 0;
}

void Z80::interrupt(uint8_t int_value)
{
    next_interrupt_data_ = int_value;
    set_INT(0);
    cycle(true);
}

void Z80::print_pin_state() const
{
    uint8_t data = fortuna1_->ram().data_bus();
    uint16_t addr = fortuna1_->ram().addr_bus();
    RAM::MemoryBus mbus = fortuna1_->ram().memory_bus();
    Z80Pins pins = state();
    char addr_s[5] = { 0 };
    char data_s[3] = { 0 };
    if ((mbus.mreq == 0 && (mbus.we == 0 || mbus.rd == 0)) || pins.iorq == 0) {
        sprintf(addr_s, "%04X", addr);
        sprintf(data_s, "%02X", data);
    } else {
        sprintf(addr_s, "----");
        sprintf(data_s, "--");
    }
    auto bit = [](bool v) { if (v) return "\e[0;32m1\e[0m"; else return "\e[0;31m0\e[0m"; };
    if (cycle_count_ % 16 == 0)
        printf_P(PSTR("ADDR DATA  MREQ WR RD  INT NMI RST BUSRQ  HALT IORQ M1 BUSAK  #CYCLE\n"));
    printf_P(PSTR("%s  %s     %s   %s  %s   %s   %s   %s    %s      %s    %s   %s    %s  %-08d\n"),
             addr_s, data_s, bit(mbus.mreq), bit(mbus.we), bit(mbus.rd),
             bit(pins.int_), bit(pins.nmi), bit(pins.rst), bit(pins.busrq),
             bit(pins.halt), bit(pins.iorq), bit(pins.m1), bit(pins.busak), cycle_count());
}

void Z80::nmi()
{
    set_NMI(0);
    step();
    step();
    set_NMI(1);
}

