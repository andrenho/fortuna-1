#ifndef COMMAND_HH_
#define COMMAND_HH_

#include <stdint.h>

#include "messages.pb.h"

class Command {
public:
    virtual ~Command() = default;
    virtual void execute(Reply& reply) = 0;

protected:
    Command() = default;
};

class CommandFreeMem : public Command {
public:
    CommandFreeMem() = default;
    void     execute(Reply& reply) override;
};

#endif
