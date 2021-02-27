#include <stdio.h>

#include <avr/pgmspace.h>

#include "ram.h"

#ifdef RUN_TESTS

#define WR_BYTE 0xea

void run_tests()
{
    // TODO - use random byte and address

    printf_P(PSTR("Write [0x42] = 0x%02X\n"), WR_BYTE);
    ram_write_byte(0x42, WR_BYTE);
    printf_P(PSTR("Read  [0x42] = 0x%02X\n\n"), ram_read_byte(0x42));
}

#endif
