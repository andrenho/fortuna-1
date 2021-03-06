#include "tests.hh"

#ifdef ENABLE_TESTS

#include <stdio.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <string.h>

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
    /*
    for (int i = 0; i < 512; ++i) {
        uint16_t addr = random() & 0xffff;
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
    */
    
    // test block with a single byte
    for (size_t sz = 28; sz < 128; sz++) {
        printf_P(PSTR("%d: "), sz);
        uint8_t buffer[sz], written[sz];
        uint16_t addr = random() & 0xffff;
        for (size_t j = 0; j < sz; ++j) {
            buffer[j] = written[j] = rand() & 0xff;
            printf_P(PSTR("%02X"), buffer[j]);
        }
        fflush(stdout);
        ram.write_block(addr, sz, [](uint16_t idx, void* buffer) -> uint8_t { return ((uint8_t *) buffer)[idx]; }, buffer);
        putchar('|');
        ram.read_block(addr, sz, [](uint16_t idx, uint8_t byte, void* buffer) { ((uint8_t *) buffer)[idx] = byte; }, buffer);
        for (size_t j = 0; j < sz; ++j)
            printf_P(PSTR("\e[0;3%dm%02X\e[0m"), (buffer[j] == written[j]) ? 2 : 1, buffer[j]);
        putchar('\n');
    }

    // create block
    /*
    printf_P(PSTR("Testing buffer...\n"));
    for (int block = 0; block < 1; ++block) {
        uint16_t addr = block == 0 ? 0 : (random() & 0x7fff);
        uint8_t buffer[256], written[256];
        for (size_t i = 0; i < 256; ++i)
            buffer[i] = rand() & 0xff;
        memcpy(written, buffer, 256);
        ram.write_block(addr, 256, [](uint16_t idx, void* buffer) -> uint8_t { return ((uint8_t *) buffer)[idx]; }, buffer);
        for (size_t i = 0; i < 256; ++i)
            buffer[i] = 0;
        ram.read_block(addr, 256, [](uint16_t idx, uint8_t byte, void* buffer) { ((uint8_t *) buffer)[idx] = byte; }, buffer);
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
     */

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
