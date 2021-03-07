#ifndef CONTROLLER_SDCARD_HH
#define CONTROLLER_SDCARD_HH

#include <stdint.h>

enum SDCardStage {
    SD_MCU_SETUP   = 0x0, SD_RESET = 0x1, SD_GO_IDLE = 0x2, SD_IF_COND = 0x3, SD_INIT = 0x4,
    SD_GET_OCR     = 0x10,
    SD_READ_OK     = 0x20, SD_READ_REJECTED = 0x21, SD_READ_TIMEOUT = 0x22, SD_READ_CRC_FAILED = 0x23,
    SD_WRITE_OK    = 0x30, SD_WRITE_REJECTED = 0x31, SD_WRITE_TIMEOUT = 0x32, SD_WRITE_DATA_REJECTED = 0x33, SD_WRITE_DATA_TIMEOUT = 0x34,
    SD_NOT_INITIALIZED = 0xff,
};

class SDCard {
public:
    bool initialize();
    void read_page(uint32_t page);
    void write_page(uint32_t page);

private:
    SDCardStage last_stage_ = SD_NOT_INITIALIZED;
    uint8_t     last_response_ = 0xff;
    
    void setup();
    void reset();
    uint8_t go_idle();
    uint8_t if_cond(uint32_t* p_int);
    uint8_t init_process(uint32_t* p_int);
};


#endif
