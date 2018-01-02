# -------------------------------------------------------------------------
# Build lightning unit tests
# -------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.3)

project( lightning_test )

# -----------------------        Test Code          -----------------------

aux_source_directory( . TEST_LIST)
include_directories(${ROOT}/src)
add_executable(${PROJECT_NAME} ${TEST_LIST})
target_link_libraries(${PROJECT_NAME} ${CONAN_LIBS} Threads::Threads)
add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})

