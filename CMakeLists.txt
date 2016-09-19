# -------------------------------------------------------------------------
# Build lightning Project
# -------------------------------------------------------------------------
# Example Usage: TODO TODO TODO TODO
# cmake .. -DCMAKE_C_COMPILER=gcc-6 -DCMAKE_CXX_COMPILER=/usr/bin/g++-6 -Dbuild_tests=true -DCMAKE_BUILD_TYPE=debug -DTESTFILES=~/testfiles/

# cmake .. -DCMAKE_C_COMPILER=gcc-6 -DCMAKE_CXX_COMPILER=/usr/bin/g++-6
#          -DCMAKE_BUILD_TYPE=debug -DNDEBUG

# -------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.2.2)

project(lightning)

SET(MAJOR_VERSION "0")
SET(MINOR_VERSION "1")
SET(PATCH_VERSION "0" CACHE PATCH_VERSION "The package build number." )

# -------------------------------------------------------------------------
# -----------------------   Options and Parameters  -----------------------
# -------------------------------------------------------------------------
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)
option(build_example_server "Build the example server." OFF)
option(build_tests "Build all http unit tests." OFF)
SET(HTTP_BUFFER_SIZE "8192" CACHE TESTFILES "The HTTP character buffer size.")
SET(HTTP_CLIENT_TIMEOUT "3" CACHE TESTFILES "the HTTP connection timeout in seconds.")
SET(HTTP_SERVER_THREAD_POOL_SIZE "10" CACHE TESTFILES "The number of threads created on the ASIO service.")
SET(TESTFILES "${PROJECT_SOURCE_DIR}/test/files" CACHE TESTFILES "The path to the testfiles.")
SET(DOCFILES "${PROJECT_BINARY_DIR}/doc/" CACHE TESTFILES "The path to the documentation files.")
add_definitions( -DHTTP_BUFFER_SIZE=${HTTP_BUFFER_SIZE} -DHTTP_CLIENT_TIMEOUT=${HTTP_CLIENT_TIMEOUT}
                 -DHTTP_SERVER_THREAD_POOL_SIZE=${HTTP_SERVER_THREAD_POOL_SIZE}
                 -DTESTFILES="${TESTFILES}/" -DDOCFILES="${DOCFILES}/"
                 -DASIO_STANDALONE -DHTTP_CPP_RELEASE="${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}" )

# -------------------------------------------------------------------------
# ---------------------   External Dependencies       ---------------------
# -------------------------------------------------------------------------

include( cmake/external/gtest.cmake )
include( cmake/external/re2.cmake )
include( cmake/external/asio.cmake )
include( cmake/external/bustache.cmake )
SET( INCLUDES ${INCLUDES} ${RE2_INCLUDE_DIR} ${ASIO_INCLUDE_DIR}
              ${BUSTACHE_INCLUDE_DIR} )

# -------------------------------------------------------------------------
# -----------------------   Find Dependencies       -----------------------
# -------------------------------------------------------------------------
find_package(Threads)
find_package(OpenSSL REQUIRED)
find_package(Boost 1.54.0)

SET(LIBS ${LIBS} ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES}
         -lssl -lcrypto -luuid -lpthread -lm ) #needed for ubuntu

SET( INCLUDES ${INCLUDES} ${Boost_LIBRARIES} ${OPENSSL_INCLUDES} )

# -------------------------------------------------------------------------
# -----------------------      Compile Code         -----------------------
# -------------------------------------------------------------------------
if( build_example_server )
    #add headers for editor
    FILE(GLOB_RECURSE HeaderFiles "src/*.h")
    add_custom_target(headers SOURCES ${HeaderFiles})

    if(CMAKE_COMPILER_IS_GNUCXX)
        add_definitions(-std=c++14)
    endif()

#    if( CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "DEBUG" )
#        message( "compile in debug mode.")
#        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
#        #set(CMAKE_C_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG} -O0 -g)
#    else()
#        message( "compile in release mode.")
#        set(CMAKE_C_FLAGS_DEBUG ${CMAKE_C_FLAGS_RELEASE} -O3)
#    endif()

    include_directories( ${ROOT}/src ${INCLUDES} )
    #create the test server
    add_executable( httpserver main.cpp )
    target_link_libraries( httpserver re bustache ${LIBS} ${CMAKE_THREAD_LIBS_INIT} )
endif()

# -------------------------------------------------------------------------
# -----------------------        Test Code          -----------------------
# -------------------------------------------------------------------------
if (build_tests)
   aux_source_directory(test TEST_LIST)
   enable_testing()
   include_directories( ${ROOT}/src ${INCLUDES} ${GTEST_INCLUDE_DIR} )
   add_executable(testmain_lightning ${TEST_LIST})
   target_link_libraries(testmain_lightning ${GMOCK_MAIN_PATH} re bustache gtest ${LIBS} )
   add_test(lightning-tests testmain_lightning)
endif()

# -------------------------------------------------------------------------
# -----------------------   Doxygen                 -----------------------
# -------------------------------------------------------------------------
find_package(Doxygen)
if( DOXYGEN_FOUND )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.bootstrap.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doc/doxy-boot.js Documentation/html/doxy-boot.js COPYONLY)
add_custom_target(doc ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen" VERBATIM
)
endif(DOXYGEN_FOUND)
