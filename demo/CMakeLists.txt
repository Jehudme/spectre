cmake_minimum_required(VERSION 3.20)
project(spectre_demo LANGUAGES C CXX)

add_executable(demo_launcher
    source/main.cpp
)

target_compile_definitions(demo_launcher PRIVATE
    APP_RESOURCES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/resources/asteroid"
)

target_link_libraries(demo_launcher
    PRIVATE
        sandbox::library
        -Wl,--whole-archive
        spectre::plugin_static
        sandbox::plugins_static
        $<TARGET_FILE:flecs_static> 
        -Wl,--no-whole-archive
)

set_target_properties(demo_launcher PROPERTIES ENABLE_EXPORTS ON)

# Use the same PCH as spectre
target_precompile_headers(demo_launcher PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_SOURCE_DIR}/spectre/source/pch.h>")
