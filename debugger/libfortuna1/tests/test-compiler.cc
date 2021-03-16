#include "compiler/compiler.hh"

#include <string>
#include <fstream>
#include <iostream>

#include "tsupport.hh"

std::string create_code(std::string const& code, int n = 1)
{
    std::string filename = "/tmp/sample" + std::to_string(n) + ".z80";
    std::ofstream f(filename);
    f.write(code.data(), code.size());
    return filename;
}

int main()
{
    std::cout << "Simple 'nop' source file (sanity check).\n";
    CompilerResult r = compile({ { create_code(" nop"), 0x0 } });
    ASSERT_Q(std::vector<uint8_t>{ 0 }, r.binary);
    ASSERT_Q((SourceAddress  { " nop", 0 }), r.debug.source.at("sample1.z80").at(0));
    ASSERT_Q((SourceLine { "sample1.z80", 0 }), r.debug.location.at(0));
}

