# -------------------------------------------------------------------------
# Build lightning unit tests
# -------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.5.0)

project( test )

# -------------------------------------------------------------------------
# ---------------------   External Dependencies       ---------------------
# -------------------------------------------------------------------------

include( ExternalProject )
include( ../cmake/external/gtest.cmake )

# -------------------------------------------------------------------------
# -----------------------        Test Code          -----------------------
# -------------------------------------------------------------------------
aux_source_directory( . TEST_LIST)
include_directories( ${ROOT}/src ${INCLUDES} ${GTEST_INCLUDE_DIR} )
add_executable(testmain_lightning ${TEST_LIST})
target_link_libraries(testmain_lightning ${GMOCK_MAIN_PATH} re2 bustache asio gtest ${LIBS} )
add_test(lightning-tests testmain_lightning)
