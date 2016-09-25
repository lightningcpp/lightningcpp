# -------------------------------------------------------------------------
# Build lightning documentation
# -------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.5.0)

project( doc )

# -------------------------------------------------------------------------
# -----------------------        Test Code          -----------------------
# -------------------------------------------------------------------------
find_package(Doxygen)
if( DOXYGEN_FOUND )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.bootstrap.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doxy-boot.js doc/doxy-boot.js COPYONLY)
add_custom_target(doc ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen" VERBATIM
)
endif(DOXYGEN_FOUND)
