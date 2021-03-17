#ifndef LIBFORTUNA1_COMPILER_HH
#define LIBFORTUNA1_COMPILER_HH

#include <optional>
#include <string>
#include <vector>
#include "debug.hh"

struct CompilerResult {
    std::optional<std::string> error;
    std::string                message;
    std::vector<uint8_t>       binary;
    Debug                      debug;
};

struct SourceFile {
    std::string filename;
    uint16_t    expected_address;
};

CompilerResult compile(std::vector<SourceFile> const& sources);

#endif //LIBFORTUNA1_COMPILER_HH