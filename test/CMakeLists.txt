# -------------------------------------------------------------------------
# Build lightning unit tests
# -------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.0)

project( test )

SET( PROJECT_TEST_NAME "lightning-tests" )

# -------------------------------------------------------------------------
# -----------------------        Test Code          -----------------------
# -------------------------------------------------------------------------
aux_source_directory( . TEST_LIST)
include_directories( ${ROOT}/src ${INCLUDES} ${GTEST_INCLUDE_DIR} )
add_executable(testmain_lightning ${TEST_LIST})
if(CMAKE_COMPILER_IS_GNUCXX)
target_compile_options(testmain_lightning PRIVATE -g -O0 -fprofile-arcs -ftest-coverage -Wall -Wextra )
endif()
target_link_libraries(testmain_lightning ${GMOCK_MAIN_PATH} re2 bustache asio gtest ${LIBS} -fprofile-arcs -ftest-coverage )
add_test(${PROJECT_TEST_NAME} testmain_lightning)
