# - download and build weighttp
# Once done this will define
#  WEIGHTTP_BINARY - The weighttp binary

ExternalProject_Add(
  weighttp_master
  SOURCE_DIR ${CMAKE_BINARY_DIR}/weighttp_master-prefix/src/weighttp_master/
  BUILD_IN_SOURCE 1
  URL "http://cgit.lighttpd.net/weighttp.git/snapshot/weighttp-master.tar.gz"
  CONFIGURE_COMMAND ${CMAKE_BINARY_DIR}/weighttp_master-prefix/src/weighttp_master/waf configure
  BUILD_COMMAND ${CMAKE_BINARY_DIR}/weighttp_master-prefix/src/weighttp_master/waf build
  INSTALL_COMMAND ""
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
)
ExternalProject_Get_Property(weighttp_master source_dir)
set(WEIGHTTP_PATH ${source_dir}/build/default/ )
set(WEIGHTTP_LIBRARY weighttp)
add_library(${WEIGHTTP_LIBRARY} INTERFACE IMPORTED)
add_dependencies(${WEIGHTTP_LIBRARY} weighttp_master)
