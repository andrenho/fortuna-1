cmake_minimum_required(VERSION 3.13)
project(libfortuna1)

find_package(Protobuf REQUIRED)
include_directories(common)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall -Wextra -I${PROJECT_SOURCE_DIR}/../../common -I${PROJECT_BINARY_DIR})

add_library(libfortuna1
            fortuna1.hh fortuna1realhardware.cc fortuna1realhardware.hh serial.cc serial.hh
            fortuna1emulator.cc fortuna1emulator.hh replyexception.hh
            libf1comm/message.hh libf1comm/message.cc libf1comm/serialization.hh libf1comm/serialization.cc libf1comm/buffer.hh
            libf1comm/defines.hh libf1comm/messages/debuginformation.hh libf1comm/messages/deserialize.hh
            libf1comm/messages/request.hh libf1comm/messages/request.cc libf1comm/messages/reply.hh libf1comm/messages/request.cc
            libf1comm/fields/ramrequest.hh libf1comm/fields/ramrequest.cc)

foreach(F IN ITEMS test-basic test-load test-memory)
    add_executable(${F} EXCLUDE_FROM_ALL tests/${F}.cc tests/tsupport.cc tests/tsupport.hh)
    target_compile_options(${F} PUBLIC -I ..)
    target_link_libraries(${F} libfortuna1 ${Protobuf_LIBRARIES})
endforeach()
