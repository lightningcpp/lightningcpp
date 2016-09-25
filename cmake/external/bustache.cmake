# - download and build bustache.
# Once done this will define
#  BUSTACHE_INCLUDE_DIR - The bustache include directory.
#  BUSTACHE_LIBRARIES - The libraries needed to use bustache.

ExternalProject_Add(
  jamboree_bustache
  URL "https://github.com/jamboree/bustache/archive/master.zip"
  CMAKE_ARGS -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER} -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER} -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}
  INSTALL_COMMAND ""
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
  BUILD_BYPRODUCTS jamboree_bustache-prefix/src/jamboree_bustache-build/libbustache.a
)
ExternalProject_Get_Property(jamboree_bustache source_dir)
set(BUSTACHE_INCLUDE_DIR ${source_dir}/include )
ExternalProject_Get_Property(jamboree_bustache binary_dir)
set(BUSTACHE_LIBRARIES ${binary_dir}/${CMAKE_FIND_LIBRARY_PREFIXES}bustache.a)
set(BUSTACHE_LIBRARY bustache)
add_library(${BUSTACHE_LIBRARY} UNKNOWN IMPORTED)
set_property(TARGET ${BUSTACHE_LIBRARY} PROPERTY IMPORTED_LOCATION ${BUSTACHE_LIBRARIES} )
add_dependencies(${BUSTACHE_LIBRARY} jamboree_bustache)
