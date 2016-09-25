include(ExternalProject)

ExternalProject_Add(
  nonius_master
  URL "https://github.com/rmartinho/nonius/archive/devel.zip"
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
)
ExternalProject_Get_Property(nonius_master source_dir)
set(NONIUS_INCLUDE_DIR ${source_dir}/include/)
set(NONIUS_LIBRARY nonius)
add_library(${NONIUS_LIBRARY} INTERFACE IMPORTED)
#set_property(TARGET ${NONIUS_LIBRARY} PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${NONIUS_INCLUDE_DIR} )
add_dependencies(${NONIUS_LIBRARY} nonius_master )
