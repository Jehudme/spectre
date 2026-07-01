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
option(SPECTRE_MODULES_STATIC "Build spectre modules statically" OFF)

include(FetchContent)

# 1. Fetch dependencies


# sandbox engine
find_package(sandbox QUIET)
if(NOT sandbox_FOUND)
    FetchContent_Declare(
        sandbox
        GIT_REPOSITORY https://github.com/Jehudme/sandbox.git
        GIT_TAG master
        GIT_SHALLOW TRUE
        EXCLUDE_FROM_ALL
    )
    FetchContent_MakeAvailable(sandbox)
endif()

# raylib
find_package(raylib QUIET)
if(NOT raylib_FOUND)
    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG master
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
        GIT_TAG main
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



# 2. Add source directories
add_subdirectory(spectre)

# 3. Add tests
include(CTest)
enable_testing()
add_subdirectory(tests)
