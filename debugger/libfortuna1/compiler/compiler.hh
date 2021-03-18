#ifndef LIBFORTUNA1_COMPILER_HH
#define LIBFORTUNA1_COMPILER_HH

#include <optional>
#include <string>
#include <vector>
#include "debug.hh"

struct Binary {
    std::vector<uint8_t> data;
    uint16_t             address;
};

struct CompilerResult {
    std::optional<std::string>              error    {};
    std::string                             message  {};
    std::unordered_map<std::string, Binary> binaries {};
    Debug                                   debug    {};
};

struct SourceFile {
    std::string filename;
    uint16_t    expected_address;
};

CompilerResult compile(std::vector<SourceFile> const& sources);

#endif //LIBFORTUNA1_COMPILER_HH