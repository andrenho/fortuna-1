#include <util/delay.h>
#include "sdcard.hh"

#define CMD0   0
#define CMD8   8
#define CMD17  17
#define CMD24  24
#define CMD55  55
#define CMD58  58
#define ACMD41 41

#define MAX_READ_ATTEMPTS 20
#define MAX_WRITE_ATTEMPTS 100

bool SDCard::initialize()
{
    last_response_ = 0xff;
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

void SDCard::reset()
{
    // powerup card
    spi_.deactivate();
    _delay_ms(1);
    for (uint8_t i = 0; i < 10; ++i)
        spi_.send(0xff);
    
    // deselect card
    spi_.deactivate();
    spi_.send(0xff);
}

uint8_t SDCard::go_idle()
{
    spi_.activate(SPI::SD);
    command(CMD0, 0, 0x94);
    uint8_t r1 = spi_.recv();
    spi_.deactivate();
    return r1;
}

uint8_t SDCard::if_cond(uint32_t* p_int)
{
    /*
    sd_cs(true);
    sd_command(CMD8, 0x1AA, 0x86);
    R1 r1 = { .value = sd_recv_spi_byte() };
    if (r1.value <= 1) {
        *response = 0;
        *response |= (uint32_t) spi_.send(0xff) << 24;
        *response |= (uint32_t) spi_.send(0xff) << 16;
        *response |= (uint32_t) spi_.send(0xff) << 8;
        *response |= (uint32_t) spi_.send(0xff);
    }
    sd_cs(false);
    return r1;
     */
    return 0;
}

uint8_t SDCard::init_process(uint32_t* p_int)
{
    /*
    sd_cs(true);
    sd_command(CMD55, 0, 0);
    R1 r1 = { .value = sd_recv_spi_byte() };
    sd_cs(false);
    if (r1.value > 1)
        return r1;
    
    sd_cs(true);
    sd_command(ACMD41, 0x40000000, 0);
    r1 = (R1) { .value = sd_recv_spi_byte() };
    if (r1.value <= 1) {
        *response = 0;
        *response |= (uint32_t) spi_.send(0xff) << 24;
        *response |= (uint32_t) spi_.send(0xff) << 16;
        *response |= (uint32_t) spi_.send(0xff) << 8;
        *response |= (uint32_t) spi_.send(0xff);
    }
    sd_cs(false);
    
    return r1;
     */
    return 0;
}

void SDCard::command(uint8_t cmd, uint32_t args, uint8_t crc)
{
    spi_.send(cmd | 0x40);
    spi_.send((uint8_t)(args >> 24));
    spi_.send((uint8_t)(args >> 16));
    spi_.send((uint8_t)(args >> 8));
    spi_.send((uint8_t)args);
    spi_.send(crc | 0x1);
}
