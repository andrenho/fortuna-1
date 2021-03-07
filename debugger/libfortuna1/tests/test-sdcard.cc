#include <iostream>

#include "tsupport.hh"

int main(int argc, char* argv[])
{
    TestArgs test_args(argc, argv);
    auto f = test_args.create_fortuna();
    
    auto status = f->sdcard_status();
    printf("Last SD card stage: 0x%02X, response: 0x%02X\n", status.last_stage, status.last_response);
    
    std::cout << "Done.\n";
}