cmake_minimum_required(VERSION 3.16)

project(simson LANGUAGES C VERSION 0.1)

set(SRCS src/simson.c)

include_directories(include)

add_executable(test_parsing tests/test_parsing.c ${SRCS})

enable_testing()

add_test(test_parsing test_parsing)
