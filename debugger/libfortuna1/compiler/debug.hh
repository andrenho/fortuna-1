#ifndef LIBFORTUNA1_DEBUG_HH
#define LIBFORTUNA1_DEBUG_HH

#include <optional>
#include <string>
#include <vector>
#include <unordered_map>
#include <ostream>
#include <iomanip>

struct SourceAddress {
    std::string             source;
    std::optional<uint16_t> address {};
    std::vector<uint8_t>    bytes   {};
    
    bool operator==(SourceAddress const& rhs) const;
    friend std::ostream& operator<<(std::ostream& os, SourceAddress const& address_);
};

struct SourceLine {
    std::string filename;
    size_t      line;
    
    bool operator==(SourceLine const& rhs) const;
    friend std::ostream& operator<<(std::ostream& os, SourceLine const& line_);
};

using NumberOfLines = size_t;

using SourceAddresses = std::unordered_map<size_t, SourceAddress>;
using Source          = std::unordered_map<std::string, SourceAddresses>;
using Location        = std::unordered_map<uint16_t, SourceLine>;
using Symbols         = std::unordered_map<std::string, uint16_t>;
using Files           = std::unordered_map<std::string, NumberOfLines>;

struct Debug {
    Source   source;
    Location location;
    Symbols  symbols;
    Files    files;
};

std::ostream& operator<<(std::ostream& os, Files const& files);

#endif