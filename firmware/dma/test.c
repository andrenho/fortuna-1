#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <avr/pgmspace.h>
#include <avr/eeprom.h>

#include "ram.h"

#ifdef RUN_TESTS

uint8_t written[256];

void run_tests()
{
    printf_P(PSTR("Buses: %04X - %02X\n"), ram_get_addr(), ram_get_data());
    struct MemoryBus mbus = ram_read_memory_bus();
    printf_P(PSTR("MREQ: %d   WE: %d   RD:%d\n"), mbus.mreq, mbus.we, mbus.rd);
    
    // create a seed
    eeprom_busy_wait();
    uint16_t seed = eeprom_read_word((uint16_t*) 0);
    srand(seed);
    srandom(seed);
    uint16_t next = rand();
    printf_P(PSTR("Seed is %d. Next is %d.\n"), seed, next);
    eeprom_write_word((uint16_t*) 0, next);

    /*
    printf_P(PSTR("Writing byte 0x0...\n"));
    ram_write_byte(0x0, 0x42);
    printf_P(PSTR("Writing byte 200x0...\n"));
    ram_write_byte(0x200, 0xfe);
    printf_P(PSTR("0x%02X == 0x42\n"), ram_read_byte(0));
    */

    printf_P(PSTR("Setting data bus..."));
    ram_set_data(0x69);
    ram_reset();
    printf_P(PSTR("ok."));

    // read/write memory byte
    for (int i = 0; i < 512; ++i) {
        uint16_t addr = random() & 0xffff;
        if (i == 0)
            addr = 0x200;
        else if (i == 1)
            addr = 0x0;
        uint8_t data = rand() & 0xff;
        printf_P(PSTR("Write [0x%04X] = 0x%02X, "), addr, data);
        ram_write_byte(addr, data);
        uint8_t data2 = ram_read_byte(addr);
        printf_P(PSTR("Read  [0x%04X] = "), addr);
        printf_P(PSTR("%s0x%02X\n\e[0m"), data == data2 ? "\e[0;32m" : "\e[0;31m", data2);
        if (data != data2)
            for(;;);
    }

    // create 512 byte block
    uint16_t addr = random() & 0xffff;

    printf_P(PSTR("Testing writing stream...\n"));
    srand(seed);
    ram_write_stream_start();
    for (size_t i = 0; i < 512; ++i) {
        uint8_t byte = rand() & 0xff;
        printf_P(PSTR("%02X"), byte);
        ram_write_byte_stream(addr + i, byte);
    }
    putchar('\n');
    ram_write_stream_end();

    printf_P(PSTR("Testing reading stream...\n"));
    srand(seed);
    ram_read_stream_start();
    for (size_t i = 0; i < 512; ++i) {
        uint8_t expected = rand() & 0xff;
        uint8_t byte = ram_read_byte_stream(addr + i);
        printf_P(PSTR("\e[0;3%dm%02X\e[0m"), (byte == expected) ? 2 : 1, byte);
    }
    putchar('\n');
    ram_read_stream_end();

    // test whole memory
    printf_P(PSTR("Testing whole memory... writing... "));

    srand(seed);
    ram_write_stream_start();
    for (uint32_t i = 0; i < 0x10000; ++i) {
        uint8_t byte = rand() & 0xff;
        ram_write_byte_stream(i, byte);
    }
    ram_write_stream_end();

    printf_P(PSTR("reading... "));
    srand(seed);
    ram_read_stream_start();
    for (uint32_t i = 0; i < 0x10000; ++i) {
        uint8_t expected = rand() & 0xff;
        uint8_t byte = ram_read_byte_stream(i);
        if (expected != byte) {
            printf_P(PSTR("\e[0;31mMismatch at address 0x%0X.\e[0m\n"), i);
            return;
        }
    }
    putchar('\n');
    ram_read_stream_end();

    printf_P(PSTR("ok!\n"));
}

#endif
