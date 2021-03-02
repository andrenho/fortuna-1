#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>

#include "tsupport.hh"

int main(int argc, char* argv[])
{
    TestArgs test_args(argc, argv);
    auto f = test_args.create_fortuna();
    
    srand(time(nullptr));
    
    ASSERT_EQ("Test communication with DMA", "Hello", f->test_dma());
    
    for (int i = 0; i < 5; ++i) {
        uint16_t addr = rand() & 0x7fff;  // TODO - using only lower bank for now
        uint8_t data = rand();
        printf("Writing byte 0x%02X to address 0x%04X...\n", data, addr);
        f->ram_write_byte(addr, data);
        ASSERT_EQ("Byte read is equal to byte written", data, f->ram_read_byte(addr));
    }
    
    /*
    {
        uint16_t addr = rand() & 0x7fff;  // TODO - using only lower bank for now
        std::vector<uint8_t> buffer;
        buffer.reserve(512);
        for (size_t i = 0; i < 512; ++i)
            buffer.push_back(rand());
        printf("Writing 512 random bytes to address 0x%04X...\n", addr);
        f->ram_write_buffer(addr, buffer);
        ASSERT_EQ("Bytes read are equal to bytes written", buffer, f->ram_read_buffer(addr, 512));
    }
     */
}

