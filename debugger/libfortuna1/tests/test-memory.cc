#include <iostream>
#include <cstdlib>
#include <ctime>

#include "tsupport.hh"

int main(int argc, char* argv[])
{
    TestArgs test_args(argc, argv);
    auto f = test_args.create_fortuna();
    
    srand(time(nullptr));
    
    ASSERT_EQ("Test communication with DMA", "Hello", f->test_dma());
    
    uint16_t addr = rand() & 0x7fff;  // TODO - using only lower bank for now
    uint8_t data = rand();
    printf("Writing byte 0x%02X to address 0x%04X...\n", data, addr);
    f->ram_write_byte(addr, data);
    ASSERT_EQ("Byte read is equal to byte written", data, f->ram_read_byte(addr));
}

