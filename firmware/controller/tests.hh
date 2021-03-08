#ifndef TESTS_HH_
#define TESTS_HH_

#ifdef ENABLE_TESTS

#include <stdbool.h>
#include "fortuna1.hh"

const char* tests_help();
bool        do_tests(char cmd, Fortuna1& fortuna1, Buffer& buffer);

#endif

#endif
