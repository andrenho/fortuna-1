#include <stdio.h>
#include <stdlib.h>

#include <avr/pgmspace.h>
#include <avr/eeprom.h>

#include "ram.h"

#ifdef RUN_TESTS

void run_tests()
{
    // create a seed
    eeprom_busy_wait();
    uint16_t seed = eeprom_read_word((uint16_t*) 0);
    srand(seed);
    uint16_t next = rand();
    printf_P(PSTR("Seed is %d. Next is %d.\n"), seed, next);
    eeprom_write_word((uint16_t*) 0, next);

    // read/write memory byte
    for (int i = 0; i < 5; ++i) {
        uint16_t addr = random() & 0x7fff;
        uint8_t data = rand() & 0xff;
        printf_P(PSTR("Write [0x%04X] = 0x%02X, "), addr, data);
        ram_write_byte(addr, data);
        uint8_t data2 = ram_read_byte(addr);
        printf_P(PSTR("Read  [0x%04X] = "));
        printf_P(PSTR("%s0x%02X\n\e[0m"), data == data2 ? "\e[0;32m" : "\e[0;31m", data2);
    }
}

#endif
