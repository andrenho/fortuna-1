#include <iostream>

#include "tsupport.hh"

int main(int argc, char* argv[])
{
    TestArgs test_args(argc, argv);
    auto f = test_args.create_fortuna();
    
    f->reset();
    ASSERT_EQ("Check that the boot sector was loaded correctly (byte 0x1fe)", 0x55, f->ram_read_byte(0x1fe));
    ASSERT_EQ("Check that the boot sector was loaded correctly (byte 0x1ff)", 0xaa, f->ram_read_byte(0x1ff));
}
