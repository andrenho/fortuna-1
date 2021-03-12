#include <iostream>

#include "tsupport.hh"

int main(int argc, char* argv[])
{
    TestArgs test_args(argc, argv);
    auto f = test_args.create_fortuna();
    
    // reset
    f->ram_write_byte(0x1fe, 0x0);
    f->ram_write_byte(0x1ff, 0x0);
    f->hard_reset();
    printf("Z80 counter: %d\n", f->z80_info().cycle_count);
    /* TODO - remove this comment once the SDCard is working again
    ASSERT_EQ("Check that the boot sector was loaded correctly (byte 0x1fe)", 0x55, f->ram_read_byte(0x1fe));
    printf("Z80 counter: %d\n", f->z80_info().cycle_count);
    ASSERT_EQ("Check that the boot sector was loaded correctly (byte 0x1ff)", 0xaa, f->ram_read_byte(0x1ff));
    printf("Z80 counter: %d\n", f->z80_info().cycle_count);
     */
    
    // set bytes with the CPU on
    printf("Writing to RAM with CPU on...\n");
    f->ram_write_byte(0, 0x42);
    f->ram_write_byte(1, 0xab);
    printf("Z80 counter: %d\n", f->z80_info().cycle_count);
    ASSERT_EQ("Check that memory was written with CPU on [byte 0]", 0x42, f->ram_read_byte(0));
    ASSERT_EQ("Check that memory was written with CPU on [byte 1]", 0xab, f->ram_read_byte(1));
    printf("Z80 counter: %d\n", f->z80_info().cycle_count);
}
