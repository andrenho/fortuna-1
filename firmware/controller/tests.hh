#ifndef TESTS_HH_
#define TESTS_HH_

#ifdef ENABLE_TESTS

#include <stdbool.h>

#include "ram.hh"

const char* tests_help();
bool        do_tests(char cmd, RAM& ram);

#endif

#endif
