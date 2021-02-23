cmake_minimum_required(VERSION 3.17)
project(libfortuna1)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall -Wextra)

add_library(libfortuna1 fortuna1.hh fortuna1realhardware.cc fortuna1realhardware.hh serial.cc serial.hh fortuna1emulator.cc fortuna1emulator.hh)

add_executable(test-basic EXCLUDE_FROM_ALL tests/test-basic.cc tests/tsupport.cc tests/tsupport.hh)
target_compile_options(test-basic PUBLIC -I ..)
target_link_libraries(test-basic libfortuna1)