#include <iostream>

#include "tsupport.hh"

int main(int argc, char* argv[])
{
    TestArgs test_args(argc, argv);
    auto f = test_args.create_fortuna();
    
    // reset
    f->ram_write_byte(0x1fe, 0x0);
    f->ram_write_byte(0x1ff, 0x0);
    ASSERT_EQ("Hard reset = OK", ResetStatus::Ok, f->hard_reset());
    printf("Z80 counter: %d\n", f->z80_info().cycle_count);
    ASSERT_EQ("Check that the boot sector was loaded correctly (byte 0x1fe)", 0x55, f->ram_read_byte(0x1fe));
    printf("Z80 counter: %d\n", f->z80_info().cycle_count);
    ASSERT_EQ("Check that the boot sector was loaded correctly (byte 0x1ff)", 0xaa, f->ram_read_byte(0x1ff));
    printf("Z80 counter: %d\n", f->z80_info().cycle_count);
    
    // set bytes with the CPU on
    printf("Writing to RAM with CPU on...\n");
    f->ram_write_byte(0, 0x42);
    f->ram_write_byte(1, 0xab);
    printf("Z80 counter: %d\n", f->z80_info().cycle_count);
    ASSERT_EQ("Check that memory was written with CPU on [byte 0]", 0x42, f->ram_read_byte(0));
    ASSERT_EQ("Check that memory was written with CPU on [byte 1]", 0xab, f->ram_read_byte(1));
    printf("Z80 counter: %d\n", f->z80_info().cycle_count);
    
    // soft reset
    f->soft_reset();
    ASSERT_EQ("Soft reset: check that memory was kept [byte 0]", 0x42, f->ram_read_byte(0));
    ASSERT_EQ("Soft reset: check that memory was kept [byte 1]", 0xab, f->ram_read_byte(1));
    
    // three NOPs
    f->ram_write_buffer(0, { 0x0, 0x0, 0xc3, 0xc3, 0xc3 });
    ASSERT_EQ("Initial state: PC == 0", 0, f->z80_info().pc);
    ASSERT_EQ("NOP: PC == 1", 1, f->z80_step().pc);
    ASSERT_EQ("NOP: PC == 2", 2, f->z80_step().pc);
    ASSERT_EQ("JP $C3C3: PC == 0xC3C3", 0xc3c3, f->z80_step().pc);
    
}
