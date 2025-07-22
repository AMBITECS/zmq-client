#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "can_master" for configuration "Debug"
set_property(TARGET can_master APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(can_master PROPERTIES
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib64/libcan_master.so.1.0.1"
  IMPORTED_SONAME_DEBUG "libcan_master.so.1"
  )

list(APPEND _cmake_import_check_targets can_master )
list(APPEND _cmake_import_check_files_for_can_master "${_IMPORT_PREFIX}/lib64/libcan_master.so.1.0.1" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
