#ifndef LIBF1COMM_BUFFER_HH
#define LIBF1COMM_BUFFER_HH

#include <stdint.h>

struct Buffer {
    uint8_t  data[512];
    uint16_t sz;
};

#endif
