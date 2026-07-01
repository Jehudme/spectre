cmake_minimum_required(VERSION 3.20)
project(spectre_unit_tests LANGUAGES C CXX)

set(SPECTRE_UNIT_TEST_SOURCES
    source/main.cpp
)

add_executable(spectre_unit_tests ${SPECTRE_UNIT_TEST_SOURCES})
set_target_properties(spectre_unit_tests PROPERTIES ENABLE_EXPORTS ON)

target_link_libraries(spectre_unit_tests
    PRIVATE
        sandbox::library
        Catch2::Catch2WithMain
        raylib
        EnTT::EnTT
)

target_include_directories(spectre_unit_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/source
        ${CMAKE_CURRENT_SOURCE_DIR}/../../spectre/source
)

# Ensure the shared libraries are built before the test executable
add_dependencies(spectre_unit_tests spectre_plugin)

# Register tests with CTest
catch_discover_tests(spectre_unit_tests WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

target_precompile_headers(spectre_unit_tests PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_SOURCE_DIR}/spectre/source/pch.h>")
