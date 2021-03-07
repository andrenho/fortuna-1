#include <util/delay.h>
#include "sdcard.hh"

bool SDCard::initialize()
{
    last_stage_ = SD_MCU_SETUP;
    last_response_ = 0x0;
    setup();
    
    last_stage_ = SD_RESET;
    reset();
    
    last_stage_ = SD_GO_IDLE;
    last_response_ = go_idle();
    if (last_response_ != 0x1)
        return false;
    
    uint32_t response = 0;
    last_stage_ = SD_IF_COND;
    last_response_ = if_cond(&response);
    if (last_response_ > 1)
        return false;
    
    last_stage_ = SD_INIT;
    for (int i = 0; i < 16; ++i) {
        last_response_ = init_process(&response);
        if (last_response_ == 0)
            return true;
        _delay_ms(50);
    }
    return false;
}

void SDCard::read_page(uint32_t page)
{

}

void SDCard::write_page(uint32_t page)
{

}

void SDCard::setup()
{

}

void SDCard::reset()
{

}

uint8_t SDCard::go_idle()
{
    return 0;
}

uint8_t SDCard::if_cond(uint32_t* p_int)
{
    return 0;
}

uint8_t SDCard::init_process(uint32_t* p_int)
{
    return 0;
}
