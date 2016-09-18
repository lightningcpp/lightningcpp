include(ExternalProject)

ExternalProject_Add(
  asio
  URL "https://github.com/chriskohlhoff/asio/archive/master.zip"
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
)
ExternalProject_Get_Property(asio source_dir)
set(ASIO_INCLUDE_DIR ${source_dir}/asio/include/)
message( ${ASIO_INCLUDE_DIR} )
