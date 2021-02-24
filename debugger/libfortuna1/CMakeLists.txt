cmake_minimum_required(VERSION 3.13)
project(libfortuna1)

find_package(Protobuf REQUIRED)
include_directories(common)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall -Wextra -I${PROJECT_SOURCE_DIR}/../../common -I${PROJECT_BINARY_DIR})

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS "${PROJECT_SOURCE_DIR}/../../common/messages.proto")
add_custom_target(proto_dep DEPENDS ${PROTO_SRCS} ${PROTO_HDRS})

add_library(libfortuna1
            fortuna1.hh fortuna1realhardware.cc fortuna1realhardware.hh serial.cc serial.hh fortuna1emulator.cc fortuna1emulator.hh ${PROTO_SRCS} ${PROTO_HDRS} replyexception.hh)
add_dependencies(libfortuna1 proto_dep)

add_executable(test-basic EXCLUDE_FROM_ALL tests/test-basic.cc tests/tsupport.cc tests/tsupport.hh)
target_compile_options(test-basic PUBLIC -I ..)
target_link_libraries(test-basic libfortuna1 ${Protobuf_LIBRARIES})
