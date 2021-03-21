#include <iostream>

#include "tsupport.hh"

int main(int argc, char* argv[])
{
    TestArgs test_args(argc, argv);
    auto f = test_args.create_fortuna();
    
    // three NOPs
    title("NOPs");
    f->ram_write_buffer(0, { 0x0, 0x0, 0xc3, 0xc3, 0xc3 });
    f->soft_reset();
    ASSERT_EQ("Initial state: PC == 0", 0, f->z80_info().pc);
    ASSERT_EQ("NOP: PC == 1", 1, f->z80_step().pc);
    ASSERT_EQ("NOP: PC == 2", 2, f->z80_step().pc);
    ASSERT_EQ("JP $C3C3: PC == 0xC3C3", 0xc3c3, f->z80_step().pc);
    
    // two-byte instruction
    title("Two-byte instruction");
    f->ram_write_buffer(0, { 0x6, 0xaf });  // LD B, $AF
    f->soft_reset();
    ASSERT_EQ("Initial state: PC == 0", 0, f->z80_info().pc);
    ASSERT_EQ("After step: PC == 2", 2, f->z80_step().pc);
    
    // last printed char
    title("Last printed char");
    Z80_Info info = run_code(f, R"(
        ld a, 'H'
        out (1), a)", 1);
    ASSERT_Q(2, info.pc);
    info = f->z80_step();
    ASSERT_Q(4, info.pc);
    std::cout << "\n";
    ASSERT_EQ("Write to string: check last printed char = 'H'", 'H', info.last_printed_char);
}
