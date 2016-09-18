# -------------------------------------------------------------------------
# Build lightning Project
# run the cmake command from your build directory:
# -------------------------------------------------------------------------
# Example Usage:
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
option(build_tests "Build all http unit tests." ON)
SET(TESTFILES "${PROJECT_SOURCE_DIR}/test/files" CACHE TESTFILES "The path to the testfiles.")
add_definitions(-DTESTFILES="${TESTFILES}/")
SET(DOCFILES "${PROJECT_BINARY_DIR}/doc/" CACHE TESTFILES "The path to the documentation files.")
add_definitions(-DDOCFILES="${DOCFILES}/")

set(MASTER_PROJECT OFF)
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  set(MASTER_PROJECT ON)
endif ()

message( "MASTER_PROJECT: " ${MASTER_PROJECT} )

add_definitions(-DASIO_STANDALONE)
add_definitions(-DHTTP_CPP_RELEASE="${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}")

# -------------------------------------------------------------------------
# ---------------------   External Dependencies       ---------------------
# -------------------------------------------------------------------------

include( cmake/external/gtest.cmake )
include( cmake/external/re2.cmake )
include( cmake/external/asio.cmake )
include( cmake/external/mstch.cmake )

SET( LIBS ${LIBS} ${MSTCH_LIBRARIES} )

#set( LIGHTNING_INCLUDES  )
#set( LIGHTNING_LIBS LIGHTNING_LIBS} )

# -------------------------------------------------------------------------
# -----------------------   Find Dependencies       -----------------------
# -------------------------------------------------------------------------
find_package(OpenSSL REQUIRED)
set(LIBS ${LIBS} ${OPENSSL_LIBRARIES})
set(LIBS ${LIBS} -lssl -lcrypto -luuid -lpthread -lm) #needed for ubuntu

# -------------------------------------------------------------------------
# -----------------------      Compile Code         -----------------------
# -------------------------------------------------------------------------
if( MASTER_PROJECT )
    #add headers for editor
    FILE(GLOB_RECURSE HeaderFiles "src/*.h")
    add_custom_target(headers SOURCES ${HeaderFiles})

    find_package(Threads)
    if(CMAKE_COMPILER_IS_GNUCXX)
        add_definitions(-std=c++14)
    endif()

    if( CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "DEBUG" )
        message( "compile in debug mode.")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
        #set(CMAKE_C_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG} -O0 -g)
    else()
        message( "compile in release mode.")
        set(CMAKE_C_FLAGS_DEBUG ${CMAKE_C_FLAGS_RELEASE} -O3)
    endif()

    include_directories( ${ROOT}/src ${ASIO_INCLUDE_DIR} ${RE2_INCLUDE_DIR} ${MSTCH_INCLUDE_DIRS} ${OPENSSL_INCLUDES} )

    #create the test server
    add_executable( httpserver main.cpp ${HTTP_SRC_LIST} )
    target_link_libraries( httpserver re mstch ${LIBS} ${CMAKE_THREAD_LIBS_INIT} )

    # -------------------------------------------------------------------------
    # -----------------------        Test Code          -----------------------
    # -------------------------------------------------------------------------
    if (build_tests)
       aux_source_directory(test TEST_LIST)
       enable_testing()
       include_directories(
           ${ROOT}
           ${GTEST_INCLUDE_DIR}
           ${ASIO_INCLUDE_DIR}
           ${OPENSSL_INCLUDES})
       add_executable(testmain_lightning ${TEST_LIST})
       target_link_libraries(testmain_lightning ${GMOCK_MAIN_PATH} re mstch gtest ${LIBS})
       add_test(lightning-tests testmain_lightning)
    endif()

    # -------------------------------------------------------------------------
    # -----------------------   Doxygen                 -----------------------
    # -------------------------------------------------------------------------
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.bootstrap.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doc/doxy-boot.js Documentation/html/doxy-boot.js COPYONLY)
    add_custom_target(doc ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen" VERBATIM
    )
    endif(DOXYGEN_FOUND)
else()
    message( "prepare variables" )
endif()
