#include <iostream>

#include "tsupport.hh"

int main(int argc, char* argv[])
{
    TestArgs test_args(argc, argv);
    auto f = test_args.create_fortuna();
    
    auto status = f->sdcard_status();
    printf("Last SD card stage: 0x%02X, response: 0x%02X\n", status.last_stage, status.last_response);
    
    auto bytes = f->sdcard_read(0);
    printf("SDCard read. Last two bytes of block 0: %02X %02X\n", bytes[510], bytes[511]);
    
    std::cout << "Done.\n";
}