#ifndef CONTROLLER_BREAKPOINTS_HH
#define CONTROLLER_BREAKPOINTS_HH

#include <stdint.h>
#include "libf1comm/defines.hh"

class Breakpoints {
public:
    bool add(uint16_t addr);
    void remove(uint16_t addr);
    void remove_all();
    
    bool contains(uint16_t addr) const;
    uint16_t const* list() const { return list_; }

private:
    uint16_t list_[MAX_BREAKPOINTS] = { 0 };
};

#endif
