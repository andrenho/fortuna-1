cmake_minimum_required(VERSION 3.17)
project(libf1comm)

set(CMAKE_CXX_STANDARD 17)

add_definitions(-DTEST)
add_executable(tests tests.cc message_type message.hh message.cc request.hh request.cc debuginformation.hh reply.hh reply.cc buffer.hh deserialize.hh serialization.cc serialization.hh)