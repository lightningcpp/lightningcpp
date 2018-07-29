# -------------------------------------------------------------------------
# Build lightning Project
# -------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.3)

project(sample)

# -----------------------      Compile Code         -----------------------
include_directories(
    ${LIGHTNING_SOURCES} ${RE2_INCLUDE_DIR}
    ${ASIO_INCLUDE_DIR} ${OPENSSL_INCLUDE_DIR}
)
#create the test server
#link_libraries("-static")
add_executable(httpserver main.cpp)
target_link_libraries(httpserver ${CONAN_LIBS} Threads::Threads)
install(TARGETS httpserver RUNTIME DESTINATION bin)

add_executable(client sample_client.cpp)
target_link_libraries(client ${CONAN_LIBS} Threads::Threads)
install(TARGETS httpserver RUNTIME DESTINATION bin)
