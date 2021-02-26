#ifndef RAM_H_
#define RAM_H_

#include <stdint.h>

extern uint8_t buffer[];

void     ram_init();

void     ram_set_addr(uint16_t addr);
uint16_t ram_get_addr();

void     ram_set_data(uint8_t data);
uint8_t  ram_get_data();

void     ram_write_byte(uint16_t addr, uint8_t data);
uint8_t  ram_read_byte(uint16_t addr);

void     ram_write_buffer(uint16_t addr, uint16_t sz);
void     ram_read_buffer(uint16_t addr, uint16_t sz);

#endif
