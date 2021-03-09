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
    // step();
}

void Z80::request_bus()
{
    if (!power_)
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

void Z80::check_iorq()
{

}

