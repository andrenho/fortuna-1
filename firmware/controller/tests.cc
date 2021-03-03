#include "tests.hh"

#ifdef ENABLE_TESTS

#include <stdio.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>

static void create_seed()
{
    // create a seed
    eeprom_busy_wait();
    uint16_t seed = eeprom_read_word((uint16_t*) 0);
    srand(seed);
    srandom(seed);
    uint16_t next = rand();
    printf_P(PSTR("Seed is %d. Next is %d.\n"), seed, next);

    eeprom_write_word((uint16_t*) 0, next);

}

static void run_memory_tests(RAM& ram)
{
    printf_P(PSTR("Running memory tests...\n"));
    // ram.spi().set_debug_mode(true);
    create_seed();

    // read/write memory byte
    for (int i = 0; i < 512; ++i) {
        uint16_t addr = random() & 0x7fff;
        uint8_t data = rand() & 0xff;
        printf_P(PSTR("Write [0x%04X] = 0x%02X, "), addr, data);
        ram.write_byte(addr, data);
        uint8_t data2 = ram.read_byte(addr);
        printf_P(PSTR("Read  [0x%04X] = "), addr);
        printf_P(PSTR("%s0x%02X\n\e[0m"), data == data2 ? "\e[0;32m" : "\e[0;31m", data2);
        if (data != data2) {
            printf_P(PSTR("Failed after %d attempts.\n"), i);
            goto done;
        }
    }

done:
    ram.spi().set_debug_mode(false);
    printf_P(PSTR("Done!\n"));
}

const char* tests_help()
{
    return PSTR("[M] Run memory tests\n");
}

bool do_tests(char cmd, RAM& ram)
{
    switch (cmd) {
        case 'M':
            run_memory_tests(ram);
            return true;
    }
    return false;
}

#endif
