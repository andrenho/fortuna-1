cmake_minimum_required(VERSION 3.13)
project(libfortuna1)

find_package(Protobuf REQUIRED)
include_directories(common)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall -Wextra -I. -I${PROJECT_BINARY_DIR})

set(LIBRARY_SOURCES
    fortuna1.hh realhardware/fortuna1realhardware.cc realhardware/fortuna1realhardware.hh realhardware/serial.cc realhardware/serial.hh
    emulator/fortuna1emulator.cc emulator/fortuna1emulator.hh replyexception.hh emulator/z80_overrides.cc)

set(EMULATOR_SOURCES
    emulator/z80/Codes.h emulator/z80/CodesCB.h emulator/z80/CodesED.h emulator/z80/CodesXCB.h emulator/z80/CodesXX.h
    emulator/z80/ConDebug.c emulator/z80/Debug.c emulator/z80/Tables.h emulator/z80/Z80.c emulator/z80/Z80.h)

set(LIBF1COMM_SOURCES
    libf1comm/message.hh libf1comm/message.cc libf1comm/serialization.hh libf1comm/serialization.cc libf1comm/buffer.hh
    libf1comm/defines.hh libf1comm/messages/debuginformation.hh libf1comm/messages/deserialize.hh
    libf1comm/messages/request.hh libf1comm/messages/request.cc libf1comm/messages/reply.hh libf1comm/messages/reply.cc
    libf1comm/fields/ramrequest.hh libf1comm/fields/ramrequest.cc emulator/sdcard_emulated.cc emulator/sdcard_emulated.hh
    libf1comm/fields/sdcardstatus.hh libf1comm/fields/sdcardstatus.cc libf1comm/fields/z80info.hh libf1comm/fields/z80info.cc)

add_library(libfortuna1 ${LIBRARY_SOURCES} ${EMULATOR_SOURCES} ${LIBF1COMM_SOURCES})

file(COPY tests/run_tests.sh DESTINATION ${PROJECT_BINARY_DIR}
     FILE_PERMISSIONS OWNER_EXECUTE OWNER_READ OWNER_WRITE WORLD_WRITE)
add_custom_target(run_tests.sh DEPENDS ${PROJECT_BINARY_DIR}/run_tests.sh)

foreach(F IN ITEMS test-basic test-load test-memory test-sdcard test-z80)
    add_executable(${F} EXCLUDE_FROM_ALL tests/${F}.cc tests/tsupport.cc tests/tsupport.hh)
    target_compile_options(${F} PUBLIC -I ..)
    target_link_libraries(${F} libfortuna1 ${Protobuf_LIBRARIES})
endforeach()
