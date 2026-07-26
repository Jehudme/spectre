cmake_minimum_required(VERSION 3.20)
project(spectre_plugin VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LUAJIT REQUIRED luajit)


include(FetchContent)

FetchContent_Declare(
    cimgui
    GIT_REPOSITORY https://github.com/cimgui/cimgui.git
    GIT_TAG docking_inter
    GIT_SUBMODULES "imgui"
)
FetchContent_Declare(
    rlimui
    GIT_REPOSITORY https://github.com/raylib-extras/rlImGui.git
    GIT_TAG Raylib_6_0
)

FetchContent_MakeAvailable(cimgui rlimui)

# Source files
set(SOURCES
    source/modules/components/components_service.cpp
    source/modules/components/components_module.cpp
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
    ${cimgui_SOURCE_DIR}/imgui/imgui.cpp
    ${cimgui_SOURCE_DIR}/imgui/imgui_draw.cpp
    ${cimgui_SOURCE_DIR}/imgui/imgui_tables.cpp
    ${cimgui_SOURCE_DIR}/imgui/imgui_widgets.cpp
    ${cimgui_SOURCE_DIR}/imgui/imgui_demo.cpp
    ${rlimui_SOURCE_DIR}/rlImGui.cpp
    ${cimgui_SOURCE_DIR}/cimgui.cpp
)

# Headers
set(HEADERS
        source/pch.h
    source/modules/components/components_module.h
    source/modules/prefabs/prefabs_module.h
    source/modules/renderer/renderer_module.h
    source/modules/resources/resources_module.h
    source/modules/runtime/runtime_module.h
    source/modules/scenes/scenes_module.h
    source/modules/scripts/scripts_module.h
    source/modules/serializer/serializer_module.h
    source/modules/window/window_module.h
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
    ${cimgui_SOURCE_DIR}/imgui
    ${cimgui_SOURCE_DIR}
    ${rlimui_SOURCE_DIR}
)

target_include_directories(spectre_plugin PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    ${LUAJIT_INCLUDE_DIRS}
)

target_include_directories(spectre_plugin_static PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/source
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${cimgui_SOURCE_DIR}/imgui
    ${cimgui_SOURCE_DIR}
    ${rlimui_SOURCE_DIR}
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
    ${LUAJIT_LIBRARIES}
)

target_link_libraries(spectre_plugin_static PRIVATE
    sandbox::library
    raylib
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
