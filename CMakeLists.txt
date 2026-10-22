cmake_minimum_required(VERSION 3.16)

project(simson LANGUAGES C VERSION 0.1)

set(SRCS src/simson.c)

include_directories(include)

add_executable(test_simson tests/test_simson.c ${SRCS})

enable_testing()

add_test(test_simson test_simson)
