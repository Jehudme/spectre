# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file LICENSE.rst or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "/home/jehud/CLionProjects/spectre/cmake-build-debug/_deps/cli11-src")
  file(MAKE_DIRECTORY "/home/jehud/CLionProjects/spectre/cmake-build-debug/_deps/cli11-src")
endif()
file(MAKE_DIRECTORY
  "/home/jehud/CLionProjects/spectre/cmake-build-debug/_deps/cli11-build"
  "/home/jehud/CLionProjects/spectre/cmake-build-debug/_deps/cli11-subbuild/cli11-populate-prefix"
  "/home/jehud/CLionProjects/spectre/cmake-build-debug/_deps/cli11-subbuild/cli11-populate-prefix/tmp"
  "/home/jehud/CLionProjects/spectre/cmake-build-debug/_deps/cli11-subbuild/cli11-populate-prefix/src/cli11-populate-stamp"
  "/home/jehud/CLionProjects/spectre/cmake-build-debug/_deps/cli11-subbuild/cli11-populate-prefix/src"
  "/home/jehud/CLionProjects/spectre/cmake-build-debug/_deps/cli11-subbuild/cli11-populate-prefix/src/cli11-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/jehud/CLionProjects/spectre/cmake-build-debug/_deps/cli11-subbuild/cli11-populate-prefix/src/cli11-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/jehud/CLionProjects/spectre/cmake-build-debug/_deps/cli11-subbuild/cli11-populate-prefix/src/cli11-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
