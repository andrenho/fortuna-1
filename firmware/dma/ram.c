#include "ram.h"

#include <avr/io.h>

extern uint8_t buffer[512] = {0};

void ram_init()
{
}

void ram_set_addr(uint16_t addr)
{
}

uint16_t ram_get_addr()
{
    return 0;
}

void ram_set_data(uint8_t data)
{
}

uint8_t ram_get_data()
{
    return 0;
}

void 
ram_write_byte(uint16_t addr, uint8_t data)
{
}

uint8_t ram_read_byte(uint16_t addr)
{
    return 0;
}

void
ram_write_buffer(uint16_t addr, uint16_t sz)
{
}

void
ram_read_buffer(uint16_t addr, uint16_t sz)
{
}
