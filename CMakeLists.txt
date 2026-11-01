cmake_minimum_required(VERSION 3.16)

project(simson LANGUAGES C VERSION 0.1.0)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(CTest)

set(SRCS src/simson.c)

add_library(simson ${SRCS})
add_library(simson::simson ALIAS simson)

target_compile_features(simson PUBLIC c_std_99)
set_target_properties(simson PROPERTIES
    C_STANDARD 99
    C_STANDARD_REQUIRED YES
    C_EXTENSIONS NO
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    PUBLIC_HEADER include/simson.h)

target_include_directories(simson PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

if(BUILD_TESTING)
    file(TO_CMAKE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/tests/data" SIMSON_TEST_DATA_DIR)

    add_executable(test_parsing tests/test_parsing.c)
    add_executable(test_serialization tests/test_serialization.c)
    add_executable(test_crud tests/test_crud.c)

    target_link_libraries(test_parsing PRIVATE simson)
    target_link_libraries(test_serialization PRIVATE simson)
    target_link_libraries(test_crud PRIVATE simson)

    target_compile_definitions(test_parsing PRIVATE
        SIMSON_TEST_DATA_DIR="${SIMSON_TEST_DATA_DIR}")

    add_test(NAME test_parsing COMMAND test_parsing)
    add_test(NAME test_serialization COMMAND test_serialization)
    add_test(NAME test_crud COMMAND test_crud)
endif()

install(TARGETS simson
    EXPORT simsonTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT simsonTargets
    FILE simsonTargets.cmake
    NAMESPACE simson::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/simson)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/simsonConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/simsonConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/simson)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/simsonConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/simsonConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/simsonConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/simson)
