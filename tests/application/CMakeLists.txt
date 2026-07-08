cmake_minimum_required(VERSION 3.20)
project(spectre_test_application LANGUAGES C CXX)

add_executable(spectre_test_application
    source/main.cpp
)

target_compile_definitions(spectre_test_application PRIVATE
    APP_RESOURCES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/resources/app"
)

target_link_libraries(spectre_test_application
    PRIVATE
        sandbox::library
        sandbox::plugins_static
        spectre::plugin_static
        -Wl,--whole-archive 
        $<TARGET_FILE:flecs_static> 
        -Wl,--no-whole-archive
)

# Use the same PCH as spectre
target_precompile_headers(spectre_test_application PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_SOURCE_DIR}/spectre/source/pch.h>")
