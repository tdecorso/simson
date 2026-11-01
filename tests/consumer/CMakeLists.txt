cmake_minimum_required(VERSION 3.16)
project(simson_consumer LANGUAGES C)

find_package(simson 0.1 CONFIG REQUIRED)

add_executable(simson_consumer ../../examples/basic.c)
target_link_libraries(simson_consumer PRIVATE simson::simson)
