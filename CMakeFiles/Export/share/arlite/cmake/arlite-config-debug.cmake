#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "arlite" for configuration "Debug"
set_property(TARGET arlite APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(arlite PROPERTIES
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libarlite.so"
  IMPORTED_SONAME_DEBUG "libarlite.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS arlite )
list(APPEND _IMPORT_CHECK_FILES_FOR_arlite "${_IMPORT_PREFIX}/lib/libarlite.so" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
