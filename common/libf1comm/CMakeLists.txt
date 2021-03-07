cmake_minimum_required(VERSION 3.17)
project(libf1comm)

set(CMAKE_CXX_STANDARD 17)

add_definitions(-DTEST)
add_executable(tests tests.cc defines.hh message.hh message.cc messages/request.hh messages/request.cc
               messages/debuginformation.hh messages/reply.hh messages/reply.cc buffer.hh messages/deserialize.hh
               serialization.cc serialization.hh fields/ramrequest.hh fields/ramrequest.cc fields/sdcardstatus.hh fields/sdcardstatus.cc)