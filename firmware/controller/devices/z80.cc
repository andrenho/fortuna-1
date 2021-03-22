#include "z80.hh"

#include "io.hh"
#include "ram.hh"

Z80::Z80(RAM& ram, Terminal& terminal)
        : ram_(ram), terminal_(terminal)
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
    
    for (int i = 0; i < 50; ++i)
        cycle();
    
    set_ZRST(1);
    power_ = true;
    cycle_count_ = 0;
    pc_ = 0;
    
    step();
    /*
    do {
        cycle();
    } while (get_M1() != 0);
    */
}

void Z80::request_bus(EachCycle f_each_cycle, void* data)
{
    if (!power_ || get_BUSACK() == 0)
        return;
    
    bool busack = 1;
    
    set_BUSREQ(0);
    while (busack == 1) {
        cycle(true, f_each_cycle, data);
        busack = get_BUSACK();
    }
    set_BUSREQ(1);
}

void Z80::cycle(bool check_iorq, EachCycle f_each_cycle, void* data)
{
    set_ZCLK(1);
    set_ZCLK(0);
    if (f_each_cycle)
        f_each_cycle(false, data);
    if (check_iorq)
        this->check_iorq(f_each_cycle, data);
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

void Z80::check_iorq(EachCycle f_each_cycle, void* data)
{
    if (get_IORQ() == 0) {
        uint16_t addr = ram_.addr_bus();
        auto m = ram_.memory_bus();
        if (m.we == 0) {  // OUT
            uint8_t ldata = ram_.data_bus();
            out(addr, ldata);
        } else if (m.rd == 0) {  // IN
            ram_.set_data_bus(in(addr));
        } else {  // INTERRUPT
            if (next_interrupt_data_ != -1) {
                ram_.set_data_bus(next_interrupt_data_ & 0xff);
                cycle();
                ram_.release_bus();
                next_interrupt_data_ = -1;
                set_INT(1);
            }
        }
        
        while (get_IORQ() == 0)
            cycle(false, f_each_cycle, data);
    }
}

void Z80::step(Z80::EachCycle f_each_cycle, void* data)
{
    terminal_.set_last_printed_char(0);
    
    bool m1 = 1;
    
    // TODO - if next instruction is extended, run two cycles
    
    while (m1 == 1) {
        cycle(true, f_each_cycle, data);
        m1 = get_M1();
    }
    pc_ = ram_.addr_bus();
    
    while (m1 == 0) {
        cycle(true, f_each_cycle, data);
        m1 = get_M1();
    }

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

void Z80::out(uint16_t addr, uint8_t data)
{
    if ((addr & 0xff) == 0x1) {     // video OUT (print char)
        terminal_.set_last_printed_char(data);
    }
}

uint8_t Z80::in(uint16_t addr)
{
    (void) addr;
    return 0;
}

void Z80::interrupt(uint8_t int_value)
{
    next_interrupt_data_ = int_value;
    set_INT(0);
    cycle(true);
}

