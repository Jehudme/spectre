cmake_minimum_required(VERSION 3.20)
project(spectre_test_application LANGUAGES C CXX)

add_executable(spectre_test_application
    source/main.cpp
)

target_compile_definitions(spectre_test_application PRIVATE
    APP_RESOURCES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/resources/asteroid"
)

target_link_libraries(spectre_test_application
    PRIVATE
        sandbox::library
        -Wl,--whole-archive
        spectre::plugin_static
        sandbox::plugins_static
        $<TARGET_FILE:flecs_static> 
        -Wl,--no-whole-archive
)

set_target_properties(spectre_test_application PROPERTIES ENABLE_EXPORTS ON)

# Use the same PCH as spectre
target_precompile_headers(spectre_test_application PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_SOURCE_DIR}/spectre/source/pch.h>")
