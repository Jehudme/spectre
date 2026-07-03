cmake_minimum_required(VERSION 3.20)
project(spectre_plugin VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SWIG REQUIRED)
include(UseSWIG)
find_package(Lua REQUIRED)

# Source files
set(SOURCES
    source/modules/renderer/renderer_module.cpp
    source/modules/resources/resource_module.cpp
        source/modules/window/window_module.cpp
        source/modules/prefabs/prefabs_module.cpp
        source/modules/prefabs/prefabs_module.h
    source/modules/scripts/scripts_module.cpp
    source/modules/scripts/scripts_module.hpp
)

# Headers
set(HEADERS
    source/modules/renderer/renderer_module.h
    source/modules/resources/resource_module.h
        include/spectre/abi/spectre_window_input_abi.h
        include/spectre/sdk/window_input_service.hpp
    include/spectre/abi/renderer_service.h
    include/spectre/abi/renderer_components.h
        source/modules/scenes/scenes_modude.cpp
        source/modules/scenes/scenes_modude.h
)

option(SPECTRE_MODULES_STATIC "Build modules statically and link them directly" OFF)

if(SPECTRE_MODULES_STATIC)
    add_library(spectre_plugin STATIC ${SOURCES} ${HEADERS})
else()
    add_library(spectre_plugin MODULE ${SOURCES} ${HEADERS})
    set_target_properties(spectre_plugin PROPERTIES
        PREFIX ""
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN ON
    )
endif()

add_library(spectre::plugin ALIAS spectre_plugin)

# Includes
target_include_directories(spectre_plugin PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/source
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(spectre_plugin PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    ${LUA_INCLUDE_DIR}
)

# Link dependencies
target_link_libraries(spectre_plugin PRIVATE
    sandbox::library
    raylib
    EnTT::EnTT
    ${LUA_LIBRARIES}
)

# SWIG Scripting Generation
set_property(SOURCE source/modules/scripts/spectre_scripting.i PROPERTY CPLUSPLUS ON)
set_property(SOURCE source/modules/scripts/spectre_scripting.i PROPERTY INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_BINARY_DIR}/_deps/flecs-src/include)
swig_add_library(spectre_scripting TYPE STATIC LANGUAGE lua SOURCES source/modules/scripts/spectre_scripting.i)
target_compile_options(spectre_scripting PRIVATE -fpermissive)
target_include_directories(spectre_scripting PRIVATE ${LUA_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_BINARY_DIR}/_deps/flecs-src/include)
target_link_libraries(spectre_scripting PRIVATE ${LUA_LIBRARIES} sandbox::library)

# Link SWIG library to plugin
target_link_libraries(spectre_plugin PRIVATE spectre_scripting)

# Copy to bin directory
add_custom_command(TARGET spectre_plugin POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    $<TARGET_FILE:spectre_plugin>
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<TARGET_FILE_NAME:spectre_plugin>
)

# Add a post-build command to copy the engine's script-facing headers and the compiled script module into the virtual directory layout "app://state/scripts/".
add_custom_command(TARGET spectre_plugin POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/app/state/scripts
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/include/spectre/abi ${CMAKE_BINARY_DIR}/app/state/scripts/include
)

target_precompile_headers(spectre_plugin PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/source/pch.h>")

# Installation rules
include(GNUInstallDirs)
install(TARGETS spectre_plugin
    EXPORT spectreTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
