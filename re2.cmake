# - Try to find RE2 or download it.
# Once done this will define
#  RE2_FOUND - System has RE2
#  RE2_INCLUDE_DIRS - The RE2 include directories
#  RE2_LIBRARIES - The libraries needed to use RE2

find_package(PkgConfig)

#message( "search path ")
#find_path(RE2_INCLUDE_DIR re2/re2.h
#    HINTS ${CMAKE_BINARY_DIR}
#    PATH_SUFFIXES re2 )
#message( "RE2 PATH: " ${RE2_INCLUDE_DIR} )
#find_library(RE2_LIBRARY NAMES re2 )

#include(FindPackageHandleStandardArgs)
## handle the QUIETLY and REQUIRED arguments and set LIBXML2_FOUND to TRUE
## if all listed variables are TRUE
#find_package_handle_standard_args(RE2  DEFAULT_MSG RE2_LIBRARY RE2_INCLUDE_DIR)

##mark_as_advanced(LIBXML2_INCLUDE_DIR LIBXML2_LIBRARY )

#if( RE2_FOUND )
#    message( "RE2_FOUND: " ${RE2_FOUND} )
#    set(RE2_LIBRARIES ${RE2_LIBRARY} )
#    set(RE2_INCLUDE_DIRS ${RE2_INCLUDE_DIR} )
#else()
    message( "RE2_NOT_FOUND: " ${RE2_FOUND} )
    ExternalProject_Add(
      re2
      URL "https://github.com/google/re2/archive/master.zip"
      INSTALL_COMMAND ""
      UPDATE_COMMAND ""
      PATCH_COMMAND ""
      BUILD_BYPRODUCTS re2-prefix/src/re2-build/libre2.a
    )
    ExternalProject_Get_Property(re2 source_dir)
    set(RE2_INCLUDE_DIR ${source_dir}/)
    ExternalProject_Get_Property(re2 binary_dir)
    set(RE2_LIBRARIES ${binary_dir}/${CMAKE_FIND_LIBRARY_PREFIXES}re2.a)
    set(RE2_LIBRARY re)
    add_library(${RE2_LIBRARY} UNKNOWN IMPORTED)
    set_property(TARGET ${RE2_LIBRARY} PROPERTY IMPORTED_LOCATION ${RE2_LIBRARIES} )
    add_dependencies(${RE2_LIBRARY} re2)

    set(RE2_LIBRARIES ${RE2_LIBRARY} )
    set(RE2_INCLUDE_DIRS ${RE2_INCLUDE_DIR} )
#endif()
