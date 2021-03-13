#ifndef RAM_H_
#define RAM_H_

#include <stdbool.h>
#include <stdint.h>

struct MemoryBus {
    bool mreq : 1;
    bool wr   : 1;
    bool rd   : 1;
};

void     ram_init();
void     ram_reset();

void     ram_set_addr(uint16_t addr);
uint16_t ram_get_addr();

void     ram_set_data(uint8_t data);
uint8_t  ram_get_data();

struct MemoryBus ram_read_memory_bus();

uint8_t  ram_write_byte(uint16_t addr, uint8_t data);
uint8_t  ram_read_byte(uint16_t addr);

void     ram_write_stream_start();
void     ram_write_byte_stream(uint16_t addr, uint8_t byte);
void     ram_write_stream_end();

void     ram_read_stream_start();
uint8_t  ram_read_byte_stream(uint16_t addr);
void     ram_read_stream_end();

#endif
