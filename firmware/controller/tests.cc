#include "tests.hh"

#include <stdio.h>
#include <avr/pgmspace.h>

#ifdef ENABLE_TESTS

const char* tests_help()
{
    return PSTR("[M] Run memory tests\n");
}

static void run_memory_tests()
{
    printf_P(PSTR("Running tests...\n"));
    printf_P(PSTR("Done!\n"));
}

bool do_tests(char cmd)
{
    switch (cmd) {
        case 'M':
            run_memory_tests();
            return true;
    }
    return false;
}

#endif
