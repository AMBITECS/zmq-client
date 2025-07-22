#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "modbus" for configuration "Debug"
set_property(TARGET modbus APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(modbus PROPERTIES
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib64/libmodbus.so.5.1.0"
  IMPORTED_SONAME_DEBUG "libmodbus.so.5"
  )

list(APPEND _cmake_import_check_targets modbus )
list(APPEND _cmake_import_check_files_for_modbus "${_IMPORT_PREFIX}/lib64/libmodbus.so.5.1.0" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
