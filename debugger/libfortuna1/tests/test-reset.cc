#include <iostream>

#include "tsupport.hh"

int main(int argc, char* argv[])
{
    TestArgs test_args(argc, argv);
    auto f = test_args.create_fortuna();
    
    // reset
    title("Reset");
    f->ram_write_byte(0x1fe, 0x0);
    f->ram_write_byte(0x1ff, 0x0);
    assert_eq("Hard reset = OK", ResetStatus::Ok, f->hard_reset());
    printf("Z80 counter: %d\n", f->z80_info().cycle_count);
    assert_eq("Check that the boot sector was loaded correctly (byte 0x1fe)", 0x55, f->ram_read_byte(0x1fe));
    printf("Z80 counter: %d\n", f->z80_info().cycle_count);
    assert_eq("Check that the boot sector was loaded correctly (byte 0x1ff)", 0xaa, f->ram_read_byte(0x1ff));
    printf("Z80 counter: %d\n", f->z80_info().cycle_count);
    
    // set bytes with the CPU on
    title("Set bytes with the CPU on");
    printf("Writing to RAM with CPU on...\n");
    f->ram_write_byte(0, 0x42);
    f->ram_write_byte(1, 0xab);
    printf("Z80 counter: %d\n", f->z80_info().cycle_count);
    assert_eq("Check that memory was written with CPU on [byte 0]", 0x42, f->ram_read_byte(0));
    assert_eq("Check that memory was written with CPU on [byte 1]", 0xab, f->ram_read_byte(1));
    printf("Z80 counter: %d\n", f->z80_info().cycle_count);
    assert_eq("PC == 0", 0, f->z80_info().pc);
    
    // soft reset
    title("Soft reset");
    f->soft_reset();
    assert_eq("Soft reset: check that memory was kept [byte 0]", 0x42, f->ram_read_byte(0));
    assert_eq("Soft reset: check that memory was kept [byte 1]", 0xab, f->ram_read_byte(1));
}


