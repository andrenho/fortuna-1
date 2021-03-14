#include "z80.hh"

#include "io.hh"

Z80::Z80(RAM& ram)
        : ram_(ram)
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
    // step();
}

void Z80::request_bus()
{
    if (!power_ || get_BUSACK() == 0)
        return;
    
    bool busack = 1;
    
    set_BUSREQ(0);
    while (busack == 1) {
        cycle();
        check_iorq();
        busack = get_BUSACK();
    }
    set_BUSREQ(1);
}

void Z80::cycle()
{
    set_ZCLK(1);
    set_ZCLK(0);
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

}

void Z80::step()
{
    bool m1 = 1;
    
    // TODO - if next instruction is extended, run two cycles
    
    while (m1 == 1) {
        cycle();
        check_iorq();
        m1 = get_M1();
    }
    pc_ = ram_.addr_bus();
    
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

