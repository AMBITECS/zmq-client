#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "pro-gateway" for configuration "Debug"
set_property(TARGET pro-gateway APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(pro-gateway PROPERTIES
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib64/libpro-gateway.so.1.0.1"
  IMPORTED_SONAME_DEBUG "libpro-gateway.so.1"
  )

list(APPEND _cmake_import_check_targets pro-gateway )
list(APPEND _cmake_import_check_files_for_pro-gateway "${_IMPORT_PREFIX}/lib64/libpro-gateway.so.1.0.1" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
