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
    // create a seed
    eeprom_busy_wait();
    uint16_t seed = eeprom_read_word((uint16_t*) 0);
    srand(seed);
    srandom(seed);
    uint16_t next = rand();
    printf_P(PSTR("Seed is %d. Next is %d.\n"), seed, next);
    eeprom_write_word((uint16_t*) 0, next);

    ram_write_byte(0x0, 0x42);
    ram_write_byte(0x200, 0xfe);
    printf_P(PSTR("0x%02X == 0x42\n"), ram_read_byte(0));

    // read/write memory byte
    for (int i = 0; i < 512; ++i) {
        uint16_t addr = random() & 0x7fff;
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
    }

    // create block
    printf_P(PSTR("Testing buffer...\n"));
    for (int block = 0; block < 2; ++block) {
        uint16_t addr = block == 0 ? 0 : (random() & 0x7fff);
        for (size_t i = 0; i < 256; ++i)
            buffer[i] = rand() & 0xff;
        memcpy(written, buffer, 256);
        ram_write_buffer(addr, 256);
        for (size_t i = 0; i < 256; ++i)
            buffer[i] = 0;
        ram_read_buffer(addr, 256);
        for (volatile size_t i = 0; i < 0x10; ++i) {
            for (volatile size_t j = 0; j < 0x10; ++j)
                printf_P(PSTR("%02X"), written[i * 0x10 + j]);
            putchar(' ');
            for (size_t j = 0; j < 0x10; ++j) {
                if (buffer[i * 0x10 + j] == written[i * 0x10 + j])
                    printf_P(PSTR("\e[0;32m"));
                else
                    printf_P(PSTR("\e[0;31m"));
                printf_P(PSTR("%02X\e[0m"), buffer[i * 0x10 + j]);
            }
            putchar('\n');
        }
        printf_P(PSTR("-\n"));
    }

    // test whole memory
    printf_P(PSTR("Testing whole memory... writing... "));
    uint8_t diff = rand();
    for (uint16_t block = 0; block < (0x8000 / 512); ++block) {
        for (size_t i = 0; i < 512; ++i)
            buffer[i] = (block + i + diff) & 0xff;
        ram_write_buffer(block * 512, 512);
    }

    printf_P(PSTR("reading... "));
    for (uint16_t block = 0; block < (0x8000 / 512); ++block) {
        ram_read_buffer(block * 512, 512);
        for (size_t i = 0; i < 512; ++i) {
            if (buffer[i] != ((block + i + diff) & 0xff)) {
                printf_P(PSTR("Failed at memory position 0x%X, expected 0x%02X but found 0x%02X.\n"), block * 512 + i, (block + i + diff) & 0xff, buffer[i]);
                printf_P(PSTR("Actual data: 0x%02X.\n"), ram_read_byte(block * 512 + i));
                for(;;);
            }
        }
    }

    printf_P(PSTR("ok!\n"));
}

#endif
