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
    
    bool operator==(SourceAddress const& rhs) const { return std::tie(source, address, bytes) == std::tie(rhs.source, rhs.address, rhs.bytes); }
    
    friend std::ostream& operator<<(std::ostream& os, SourceAddress const& address_)
    {
        os << std::hex << std::uppercase << std::setfill('0') << std::setw(2);
        os << "source: \"" << address_.source << "\" address: " << address_.address.value_or(-1) << " bytes: ";
        for (uint8_t b: address_.bytes)
            os << (int) b << ' ';
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