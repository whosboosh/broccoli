#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "spirv-cross-c-shared" for configuration ""
set_property(TARGET spirv-cross-c-shared APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(spirv-cross-c-shared PROPERTIES
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib64/libspirv-cross-c-shared.so.0.48.0"
  IMPORTED_SONAME_NOCONFIG "libspirv-cross-c-shared.so.0"
  )

list(APPEND _cmake_import_check_targets spirv-cross-c-shared )
list(APPEND _cmake_import_check_files_for_spirv-cross-c-shared "${_IMPORT_PREFIX}/lib64/libspirv-cross-c-shared.so.0.48.0" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
