cmake_minimum_required(VERSION 3.20)
project(spectre_unit_tests LANGUAGES C CXX)

set(SPECTRE_UNIT_TEST_SOURCES
    source/main.cpp
    source/resources_test.cpp
)

add_executable(spectre_unit_tests ${SPECTRE_UNIT_TEST_SOURCES})
set_target_properties(spectre_unit_tests PROPERTIES ENABLE_EXPORTS ON)

target_compile_definitions(spectre_unit_tests PRIVATE
    APP_RESOURCES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/../application/resources/app"
)

target_link_libraries(spectre_unit_tests
    PRIVATE
        sandbox::library
        -Wl,--whole-archive 
        $<TARGET_FILE:sandbox_plugin>
        $<TARGET_FILE:spectre_plugin>
        $<TARGET_FILE:flecs_static> 
        -Wl,--no-whole-archive
        sandbox_plugin
        spectre_plugin
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
