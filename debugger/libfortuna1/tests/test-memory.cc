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
    
    for (int i = 0; i < 32; ++i) {
        std::cout << "Attempt #" << i << "\n";
        uint16_t addr = rand() & 0x7fff;  // TODO - using only lower bank for now
        uint8_t data = rand();
        printf("Writing byte 0x%02X to address 0x%04X...\n", data, addr);
        f->ram_write_byte(addr, data);
        ASSERT_EQ("Byte read is equal to byte written", data, f->ram_read_byte(addr));
    }
    
    {
        uint16_t addr = 0x4365;
        std::vector<uint8_t> buffer;
        buffer.push_back(0x42);
        buffer.push_back(0xFE);
        f->ram_write_byte(addr, 8);
        printf("Writing buffer...\n");
        f->ram_write_buffer(addr, buffer);
        ASSERT_EQ("Checking byte 0", 0x42, f->ram_read_byte(addr));
        ASSERT_EQ("Checking byte 1'", 0xfe, f->ram_read_byte(addr + 1));
        auto cbuffer = f->ram_read_buffer(addr, 2);
        ASSERT_EQ("Verifying byte written (0)", 0x42, cbuffer.at(0));
        ASSERT_EQ("Verifying byte written (1)", 0xfe, cbuffer.at(1));
    }
    
    for (int i = 0; i < 8; ++i)
    {
        uint16_t addr = rand() & 0xffff;
        std::vector<uint8_t> buffer;
        buffer.reserve(512);
        for (size_t i = 0; i < 512; ++i)
            buffer.push_back(rand());
        printf("Writing 512 random bytes to address 0x%04X...\n", addr);
        f->ram_write_buffer(addr, buffer);
        ASSERT_EQ("Bytes read are equal to bytes written", buffer, f->ram_read_buffer(addr, 512));
    }
    
}
