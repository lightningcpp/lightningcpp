# -------------------------------------------------------------------------
# Build lightning unit tests
# -------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.3)

project( lightning_test )

SET( PROJECT_TEST_NAME "LightningTests" )

# -------------------------------------------------------------------------
# -----------------------        Test Code          -----------------------
# -------------------------------------------------------------------------
aux_source_directory( . TEST_LIST)
include_directories(
    ${ROOT}/src ${RE2_INCLUDE_DIR} ${ASIO_INCLUDE_DIR}
    ${GTEST_INCLUDE_DIR} ${OPENSSL_INCLUDE_DIR}
)
add_executable(${PROJECT_NAME} ${TEST_LIST})
target_link_libraries(
    ${PROJECT_NAME} ${GTEST_MAIN_PATH} re2 asio
    gtest ${LIBS} ssl crypto -ldl Threads::Threads
)
add_test(NAME ${PROJECT_TEST_NAME} COMMAND ${PROJECT_NAME})

