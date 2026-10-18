cmake_minimum_required(VERSION 3.16)

project(json LANGUAGES C VERSION 0.1)

set(SRCS src/json.c)

include_directories(include)

add_executable(test_json tests/test_json.c ${SRCS})

enable_testing()

add_test(test_json test_json)
