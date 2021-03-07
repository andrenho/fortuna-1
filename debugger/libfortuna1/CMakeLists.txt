cmake_minimum_required(VERSION 3.13)
project(libfortuna1)

find_package(Protobuf REQUIRED)
include_directories(common)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall -Wextra -I. -I${PROJECT_BINARY_DIR})

add_library(libfortuna1
            fortuna1.hh realhardware/fortuna1realhardware.cc realhardware/fortuna1realhardware.hh realhardware/serial.cc realhardware/serial.hh
            emulator/fortuna1emulator.cc emulator/fortuna1emulator.hh replyexception.hh
            libf1comm/message.hh libf1comm/message.cc libf1comm/serialization.hh libf1comm/serialization.cc libf1comm/buffer.hh
            libf1comm/defines.hh libf1comm/messages/debuginformation.hh libf1comm/messages/deserialize.hh
            libf1comm/messages/request.hh libf1comm/messages/request.cc libf1comm/messages/reply.hh libf1comm/messages/reply.cc
            libf1comm/fields/ramrequest.hh libf1comm/fields/ramrequest.cc emulator/sdcard_emulated.cc emulator/sdcard_emulated.hh
            libf1comm/fields/sdcardstatus.hh libf1comm/fields/sdcardstatus.cc)

foreach(F IN ITEMS test-basic test-load test-memory test-sdcard)
    add_executable(${F} EXCLUDE_FROM_ALL tests/${F}.cc tests/tsupport.cc tests/tsupport.hh)
    target_compile_options(${F} PUBLIC -I ..)
    target_link_libraries(${F} libfortuna1 ${Protobuf_LIBRARIES})
endforeach()
