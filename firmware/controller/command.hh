#ifndef COMMAND_HH_
#define COMMAND_HH_

#include "serial.hh"
#include "ram.hh"

class Command {
public:
    Command(Serial const& serial, RAM& ram) : serial_(serial), ram_(ram) {}

    int         free_ram() const;
    void        test_debug_messages() const;
    const char* test_dma() const;

private:
    Serial const& serial_;
    RAM&          ram_;
};

#endif
