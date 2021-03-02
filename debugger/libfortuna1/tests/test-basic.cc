#include <iostream>

#include "tsupport.hh"

int main(int argc, char* argv[])
{
    TestArgs test_args(argc, argv);
    auto f = test_args.create_fortuna();
    
    ASSERT_GT("Test memory amount", f->free_mem(), 0UL);
    
    /*
    std::cout << "Testing debug messages...\n";
    f->test_debug_messages();
    std::cout << "Done.\n";
     */
}
