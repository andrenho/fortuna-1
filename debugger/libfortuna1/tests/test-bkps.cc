#include <iostream>

#include "tsupport.hh"
#include "../libf1comm/defines.hh"

#include <vector>

int main(int argc, char* argv[])
{
    TestArgs test_args(argc, argv);
    auto f = test_args.create_fortuna();
    
    assert_eq("Initialization: breakpoint empty list", true, f->list_breakpoints().empty());
    
    assert_eq("Add one breakpoint", 0xf412, f->add_breakpoint(0xf412).at(0));
    f->add_breakpoint(0x4388);
    f->add_breakpoint(0x12);
    assert_contains("Check if all breakpoints were added",
                    std::vector<uint16_t>{ 0x12, 0x4388, 0xf412 }, f->list_breakpoints());
    
    f->add_breakpoint(0x4388);
    assert_contains("Check that breakpoints are not repeating",
                    std::vector<uint16_t> { 0x12, 0x4388, 0xf412 }, f->list_breakpoints());
    
    assert_contains("Check that breakpoints can be removed", std::vector<uint16_t>(0x12, 0xf412), f->remove_breakpoint(0x4388));
    
    assert_eq("Empty all breakpoints", true, f->remove_all_breakpoints().empty());
    
    for (size_t i = 0; i < MAX_BREAKPOINTS; ++i)
        f->add_breakpoint(i + 1);
    assert_throws("Check that we can't add more than the max breakpoints", [&](){ f->add_breakpoint(0xffff); });
}