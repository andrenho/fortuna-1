#ifndef LIBFORTUNA1_DEBUG_HH
#define LIBFORTUNA1_DEBUG_HH

#include <optional>
#include <string>
#include <vector>
#include <unordered_map>

struct SourceAddress {
    std::string             source;
    std::optional<uint16_t> address;
};

struct SourceLine {
    std::string source;
    size_t      line;
};

class Debug {
private:
    std::unordered_map<std::string, std::vector<SourceAddress>> source;
    std::unordered_map<uint16_t, SourceLine>                    location;
    std::unordered_map<std::string, uint16_t>                   symbols;
};

#endif