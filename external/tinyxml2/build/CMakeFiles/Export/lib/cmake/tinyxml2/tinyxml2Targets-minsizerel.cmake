#----------------------------------------------------------------
# Generated CMake target import file for configuration "MinSizeRel".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "tinyxml2_static" for configuration "MinSizeRel"
set_property(TARGET tinyxml2_static APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)
set_target_properties(tinyxml2_static PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_MINSIZEREL "CXX"
  IMPORTED_LOCATION_MINSIZEREL "${_IMPORT_PREFIX}/lib/tinyxml2.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS tinyxml2_static )
list(APPEND _IMPORT_CHECK_FILES_FOR_tinyxml2_static "${_IMPORT_PREFIX}/lib/tinyxml2.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
