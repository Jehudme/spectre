cmake_minimum_required(VERSION 3.20)
project(spectre_unit_tests LANGUAGES C CXX)

set(SPECTRE_UNIT_TEST_SOURCES
    source/main.cpp
    source/modules/test_components.cpp
    source/modules/test_serializer.cpp
    source/modules/test_prefabs.cpp
    source/modules/test_scripts.cpp
    source/modules/test_scenes.cpp
    source/modules/test_resources.cpp
    source/modules/test_window.cpp
    source/modules/test_renderer.cpp
    source/modules/test_lua_api.cpp
)

add_executable(spectre_unit_tests ${SPECTRE_UNIT_TEST_SOURCES})
set_target_properties(spectre_unit_tests PROPERTIES ENABLE_EXPORTS ON)

target_compile_definitions(spectre_unit_tests PRIVATE
    APP_RESOURCES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/../application/resources/asteroid"
)

target_link_libraries(spectre_unit_tests
    PRIVATE
        -Wl,--whole-archive 
        spectre::plugin_static
        sandbox::plugins_static
        sandbox::library
        $<TARGET_FILE:flecs_static> 
        -Wl,--no-whole-archive
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
add_dependencies(spectre_unit_tests spectre_plugin spectre_plugin_static sandbox_plugin_static)

# Register tests with CTest
catch_discover_tests(spectre_unit_tests WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

target_precompile_headers(spectre_unit_tests PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_SOURCE_DIR}/spectre/source/pch.h>")
