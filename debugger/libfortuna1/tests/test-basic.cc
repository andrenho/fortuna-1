#include <iostream>

#include "tsupport.hh"

int main(int argc, char* argv[])
{
    TestArgs t(argc, argv);
    auto f = t.create_fortuna();
    
    assert_gt("Test memory amount", f->free_mem(), 0UL);
    
    std::cout << "Testing debug messages...\n";
    f->test_debug_messages();
    
    std::cout << "Done.\n";
}
