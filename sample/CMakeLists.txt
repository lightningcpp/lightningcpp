# -------------------------------------------------------------------------
# Build lightning Project
# -------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.3)

project( sample )

# -------------------------------------------------------------------------
# -----------------------      Compile Code         -----------------------
# -------------------------------------------------------------------------

include_directories( ${LIGHTNING_SOURCES} ${INCLUDES} )
#create the test server
add_executable( httpserver main.cpp )
target_link_libraries( httpserver re2 bustache asio ${LIBS} ${CMAKE_THREAD_LIBS_INIT} )
