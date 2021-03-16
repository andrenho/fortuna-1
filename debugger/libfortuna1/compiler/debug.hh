#ifndef LIBFORTUNA1_DEBUG_HH
#define LIBFORTUNA1_DEBUG_HH

#include <optional>
#include <string>
#include <vector>
#include <unordered_map>
#include <ostream>

struct SourceAddress {
    std::string             source;
    std::optional<uint16_t> address;
    
    bool operator==(SourceAddress const& rhs) const { return std::tie(source, address) == std::tie(rhs.source, rhs.address); }
    
    friend std::ostream& operator<<(std::ostream& os, SourceAddress const& address_)
    {
        os << "source: " << address_.source << " address_: " << address_.address.value_or(-1);
        return os;
    }
};

struct SourceLine {
    std::string filename;
    size_t      line;
    
    bool operator==(SourceLine const& rhs) const { return std::tie(filename, line) == std::tie(rhs.filename, rhs.line); }
    
    friend std::ostream& operator<<(std::ostream& os, SourceLine const& line_)
    {
        os << "filename: " << line_.filename << " line_: " << line_.line;
        return os;
    }
};

using SourceAddresses = std::vector<SourceAddress>;
using Source = std::unordered_map<std::string, SourceAddresses>;
using Location = std::unordered_map<uint16_t, SourceLine>;
using Symbols = std::unordered_map<std::string, uint16_t>;

struct Debug {
    Source   source;
    Location location;
    Symbols  symbols;
};



#endif