#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "sandbox::sandbox_library" for configuration "Debug"
set_property(TARGET sandbox::sandbox_library APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(sandbox::sandbox_library PROPERTIES
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libsandbox_library.so"
  IMPORTED_SONAME_DEBUG "libsandbox_library.so"
  )

list(APPEND _cmake_import_check_targets sandbox::sandbox_library )
list(APPEND _cmake_import_check_files_for_sandbox::sandbox_library "${_IMPORT_PREFIX}/lib/libsandbox_library.so" )

# Import target "sandbox::sandbox_plugin" for configuration "Debug"
set_property(TARGET sandbox::sandbox_plugin APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(sandbox::sandbox_plugin PROPERTIES
  IMPORTED_COMMON_LANGUAGE_RUNTIME_DEBUG ""
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/sandbox_plugin.so"
  IMPORTED_NO_SONAME_DEBUG "TRUE"
  )

list(APPEND _cmake_import_check_targets sandbox::sandbox_plugin )
list(APPEND _cmake_import_check_files_for_sandbox::sandbox_plugin "${_IMPORT_PREFIX}/lib/sandbox_plugin.so" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
