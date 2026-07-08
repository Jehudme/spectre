cmake_minimum_required(VERSION 3.20)
project(spectre_plugin VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Lua REQUIRED)

# Source files
set(SOURCES
    source/spectre.cpp
    source/modules/prefabs/prefabs_module.cpp
    source/modules/renderer/renderer_module.cpp
    source/modules/resources/resources_module.cpp
    source/modules/scenes/scenes_module.cpp
    source/modules/scripts/scripts_module.cpp
    source/modules/serializer/serializer_module.cpp
    source/modules/window/window_module.cpp
)

# Headers
set(HEADERS
    include/spectre/components.h
    include/spectre/services.h
    include/spectre/spectre.h
    source/pch.h
    source/modules/prefabs/prefabs_module.h
    source/modules/renderer/renderer_module.h
    source/modules/resources/resources_module.h
    source/modules/scenes/scenes_module.h
    source/modules/scripts/scripts_module.h
    source/modules/serializer/serializer_module.h
    source/modules/window/window_module.h
    source/utilities/hashing.h
    source/utilities/properties_helpers.h
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

# Copy to bin directory
add_custom_command(TARGET spectre_plugin POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    $<TARGET_FILE:spectre_plugin>
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<TARGET_FILE_NAME:spectre_plugin>
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
