#ifndef LIBFORTUNA1_SDCARD_EMULATED_HH
#define LIBFORTUNA1_SDCARD_EMULATED_HH

#include <cstdint>

class SDCardEmulated {
public:
    uint8_t last_stage() const { return last_stage_; }
    uint8_t last_response() const { return last_response_; }

private:
    uint8_t last_stage_ = 0xff;
    uint8_t last_response_ = 0xff;
};

#endif
