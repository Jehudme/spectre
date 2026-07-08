cmake_minimum_required(VERSION 3.20)
project(spectre_workspace VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Configure output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

option(BUILD_SHARED_LIBS "Build shared libraries" ON)
set(SPECTRE_MODULES_STATIC ON CACHE BOOL "Build spectre modules statically" FORCE)
set(SANDBOX_MODULES_STATIC ON CACHE BOOL "Build sandbox modules statically" FORCE)

include(FetchContent)

# 1. Fetch dependencies
set(SPECTRE_ORIGINAL_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})
set(BUILD_SHARED_LIBS OFF)


# sandbox engine
find_package(sandbox QUIET)
if(NOT sandbox_FOUND)
    FetchContent_Declare(
        sandbox
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../sandbox
    )
    FetchContent_MakeAvailable(sandbox)
endif()

# raylib
find_package(raylib QUIET)
if(NOT raylib_FOUND)
    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 5.0 # Locked to specific tag
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(raylib)
endif()

# EnTT
find_package(EnTT QUIET)
if(NOT EnTT_FOUND)
    FetchContent_Declare(
        entt
        GIT_REPOSITORY https://github.com/skypjack/entt.git
        GIT_TAG v3.13.2 # Locked to specific tag
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(entt)
endif()

# Catch2 (for tests)
find_package(Catch2 3.8.1 QUIET)
if(NOT Catch2_FOUND)
    FetchContent_Declare(
        catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.8.1
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(catch2)
endif()



set(BUILD_SHARED_LIBS ${SPECTRE_ORIGINAL_BUILD_SHARED_LIBS})

# 2. Add source directories
add_subdirectory(spectre)

# 3. Add tests
include(CTest)
enable_testing()
add_subdirectory(tests)
