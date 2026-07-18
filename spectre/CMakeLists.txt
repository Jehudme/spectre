cmake_minimum_required(VERSION 3.20)
project(spectre_plugin VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LUAJIT REQUIRED luajit)

# Scripts code generation
set(FFI_INPUT_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/source/modules/scripts/spectre_ffi_api.h")
set(FFI_OUTPUT_HEADER "${CMAKE_SOURCE_DIR}/tests/application/resources/app/resources/assets/scripts/declarations/spectre_api.h")
add_custom_command(
    OUTPUT ${FFI_OUTPUT_HEADER} "${CMAKE_SOURCE_DIR}/tests/application/resources/app/resources/assets/scripts/declarations/stubs/spectre_api.lua"
    COMMAND python3 "${CMAKE_CURRENT_SOURCE_DIR}/source/modules/scripts/generate_ffi_header.py" 
            ${FFI_INPUT_HEADER} 
            ${FFI_OUTPUT_HEADER}
            "${CMAKE_CURRENT_SOURCE_DIR}/../../sandbox/modules/core/include"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../sandbox/sandbox/include"
            "${CMAKE_CURRENT_SOURCE_DIR}/include"
            "${CMAKE_BINARY_DIR}/_deps/flecs-src/distr"
            "${CMAKE_BINARY_DIR}/_deps/flecs-src/include"
    COMMAND python3 "${CMAKE_CURRENT_SOURCE_DIR}/source/modules/scripts/generate_ffi_stubs.py"
            ${FFI_OUTPUT_HEADER}
            "${CMAKE_SOURCE_DIR}/tests/application/resources/app/resources/assets/scripts/declarations/stubs/spectre_api.lua"
    DEPENDS ${FFI_INPUT_HEADER} "${CMAKE_CURRENT_SOURCE_DIR}/source/modules/scripts/generate_ffi_header.py" "${CMAKE_CURRENT_SOURCE_DIR}/source/modules/scripts/generate_ffi_stubs.py"
    COMMENT "Generating FFI cdef header and Lua stubs for Lua scripts"
)
add_custom_target(generate_ffi_header ALL DEPENDS ${FFI_OUTPUT_HEADER} "${CMAKE_SOURCE_DIR}/tests/application/resources/app/resources/assets/scripts/declarations/stubs/spectre_api.lua")

# Source files
set(SOURCES
    source/modules/prefabs/prefabs_service.cpp
    source/modules/prefabs/prefabs_module.cpp
    source/modules/renderer/renderer_service.cpp
    source/modules/renderer/renderer_module.cpp
    source/modules/resources/resources_service.cpp
    source/modules/resources/resources_module.cpp
    source/modules/runtime/runtime_module.cpp
    source/modules/scenes/scenes_service.cpp
    source/modules/scenes/scenes_module.cpp
    source/modules/scripts/scripts_service.cpp
    source/modules/scripts/scripts_module.cpp
    source/modules/serializer/serializer_service.cpp
    source/modules/serializer/serializer_module.cpp
    source/modules/window/window_service.cpp
    source/modules/window/window_module.cpp
)

# Headers
set(HEADERS
        source/pch.h
    source/modules/prefabs/prefabs_module.h
    source/modules/renderer/renderer_module.h
    source/modules/resources/resources_module.h
    source/modules/runtime/runtime_module.h
    source/modules/scenes/scenes_module.h
    source/modules/scripts/scripts_module.h
    source/modules/serializer/serializer_module.h
    source/modules/window/window_module.h
    source/utilities/hashing.h
    source/utilities/properties_helpers.h
)

add_library(spectre_plugin MODULE ${SOURCES} ${HEADERS})
set_target_properties(spectre_plugin PROPERTIES
    PREFIX ""
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
)
add_library(spectre_plugin_static STATIC ${SOURCES} ${HEADERS})

add_library(spectre::plugin ALIAS spectre_plugin)
add_library(spectre::plugin_static ALIAS spectre_plugin_static)

# Includes
target_include_directories(spectre_plugin PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/source
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(spectre_plugin PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    ${LUAJIT_INCLUDE_DIRS}
)

target_include_directories(spectre_plugin_static PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/source
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(spectre_plugin_static PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    ${LUAJIT_INCLUDE_DIRS}
)

# Link dependencies
target_link_libraries(spectre_plugin PRIVATE
    sandbox::library
    raylib
    EnTT::EnTT
    ${LUAJIT_LIBRARIES}
)

target_link_libraries(spectre_plugin_static PRIVATE
    sandbox::library
    raylib
    EnTT::EnTT
    ${LUAJIT_LIBRARIES}
)

# Copy to bin directory
add_custom_command(TARGET spectre_plugin POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    $<TARGET_FILE:spectre_plugin>
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<TARGET_FILE_NAME:spectre_plugin>
)

target_precompile_headers(spectre_plugin PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/source/pch.h>")
target_precompile_headers(spectre_plugin_static PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/source/pch.h>")

# Installation rules
include(GNUInstallDirs)
install(TARGETS spectre_plugin
    EXPORT spectreTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
