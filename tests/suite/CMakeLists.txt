cmake_minimum_required(VERSION 3.20)
project(spectre_suite_tests LANGUAGES C CXX)

set(SPECTRE_SUITE_TEST_SOURCES
    source/main.cpp
)

add_executable(spectre_suite_tests ${SPECTRE_SUITE_TEST_SOURCES})
set_target_properties(spectre_suite_tests PROPERTIES ENABLE_EXPORTS ON)

target_link_libraries(spectre_suite_tests
    PRIVATE
        sandbox::library
        Catch2::Catch2WithMain
        raylib
        EnTT::EnTT
)

target_include_directories(spectre_suite_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/source
        ${CMAKE_CURRENT_SOURCE_DIR}/../../spectre/source
)

# Ensure the shared libraries are built before the test executable
add_dependencies(spectre_suite_tests spectre_plugin)

# Register tests with CTest
catch_discover_tests(spectre_suite_tests WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

target_precompile_headers(spectre_suite_tests PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_SOURCE_DIR}/spectre/source/pch.h>")
