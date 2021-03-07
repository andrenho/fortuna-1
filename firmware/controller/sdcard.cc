#include <util/delay.h>
#include "sdcard.hh"

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
    /*
    // power up card
    sd_cs(false);
    _delay_ms(1);
    for (uint8_t i = 0; i < 10; ++i)
        sd_send_spi_byte(0xff);
    
    // deselect SD card
    sd_cs(false);
    sd_send_spi_byte(0xff);
     */
}

uint8_t SDCard::go_idle()
{
    /*
    sd_cs(true);
    sd_command(CMD0, 0, 0x94);
    R1 r1 = { .value = sd_recv_spi_byte() };
    sd_cs(false);
    return r1;
     */
    return 0;
}

uint8_t SDCard::if_cond(uint32_t* p_int)
{
    /*
    sd_cs(true);
    sd_command(CMD8, 0x1AA, 0x86);
    R1 r1 = { .value = sd_recv_spi_byte() };
    if (r1.value <= 1) {
        *response = 0;
        *response |= (uint32_t) sd_send_spi_byte(0xff) << 24;
        *response |= (uint32_t) sd_send_spi_byte(0xff) << 16;
        *response |= (uint32_t) sd_send_spi_byte(0xff) << 8;
        *response |= (uint32_t) sd_send_spi_byte(0xff);
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
        *response |= (uint32_t) sd_send_spi_byte(0xff) << 24;
        *response |= (uint32_t) sd_send_spi_byte(0xff) << 16;
        *response |= (uint32_t) sd_send_spi_byte(0xff) << 8;
        *response |= (uint32_t) sd_send_spi_byte(0xff);
    }
    sd_cs(false);
    
    return r1;
     */
    return 0;
}