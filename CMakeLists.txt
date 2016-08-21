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

if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions(-std=c++14)
endif()

find_package(Threads)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

set(CMAKE_C_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG} -O0 -g)
add_definitions(-DASIO_STANDALONE)
add_definitions(-DHTTP_CPP_RELEASE="${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}")
add_definitions(-DTESTFILES="${TESTFILES}/")

# -------------------------------------------------------------------------
# ---------------------   External Dependencies       ---------------------
# -------------------------------------------------------------------------
include(ExternalProject)

ExternalProject_Add(
  googletest
  URL "https://github.com/google/googletest/archive/master.zip"
  INSTALL_COMMAND ""
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
)
ExternalProject_Get_Property(googletest source_dir)
set(GTEST_INCLUDE_DIR ${source_dir}/googletest/include/ ${source_dir}/googlemock/include/)
ExternalProject_Get_Property(googletest binary_dir)
set(GTEST_LIBRARY_PATH ${binary_dir}/googlemock/gtest/${CMAKE_FIND_LIBRARY_PREFIXES}gtest.a )
set(GMOCK_LIBRARY_PATH ${binary_dir}/googlemock/${CMAKE_FIND_LIBRARY_PREFIXES}gmock.a )
set(GTEST_MAIN_PATH ${binary_dir}/googlemock/gtest/${CMAKE_FIND_LIBRARY_PREFIXES}gtest_main.a )
set(GMOCK_MAIN_PATH ${binary_dir}/googlemock/${CMAKE_FIND_LIBRARY_PREFIXES}gmock_main.a )
set(GTEST_LIBRARY gtest)
add_library(${GTEST_LIBRARY} UNKNOWN IMPORTED)
set_property(TARGET ${GTEST_LIBRARY} PROPERTY IMPORTED_LOCATION ${GTEST_LIBRARY_PATH} )
set_property(TARGET ${GTEST_LIBRARY} PROPERTY IMPORTED_LOCATION ${GMOCK_LIBRARY_PATH} )
add_dependencies(${GTEST_LIBRARY} googletest)

ExternalProject_Add(
  re2
  URL "https://github.com/google/re2/archive/master.zip"
  INSTALL_COMMAND ""
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
)
ExternalProject_Get_Property(re2 source_dir)
set(RE2_INCLUDE_DIR ${source_dir}/)
ExternalProject_Get_Property(re2 binary_dir)
set(RE2_LIBRARY_PATH ${binary_dir}/${CMAKE_FIND_LIBRARY_PREFIXES}re2.a)
set(RE2_LIBRARY re)
add_library(${RE2_LIBRARY} UNKNOWN IMPORTED)
set_property(TARGET ${RE2_LIBRARY} PROPERTY IMPORTED_LOCATION ${RE2_LIBRARY_PATH} )
add_dependencies(${RE2_LIBRARY} re2)

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

# -------------------------------------------------------------------------
# -----------------------   Find Dependencies       -----------------------
# -------------------------------------------------------------------------
find_package(OpenSSL REQUIRED)
set(LIBS ${LIBS} ${OPENSSL_LIBRARIES})
set(LIBS ${LIBS} -lssl -lcrypto -luuid -lpthread -lm) #needed for ubuntu

# -------------------------------------------------------------------------
# -----------------------      Compile Code         -----------------------
# -------------------------------------------------------------------------
aux_source_directory(src HTTP_SRC_LIST)
set( HTTP_SRC_LIST ${HTTP_SRC_LIST} src/utils/stringutils.h src/utils/base64.cpp src/utils/httpparser.cpp src/utils/chunked.cpp)
set( HTTP_SRC_LIST ${HTTP_SRC_LIST} src/httpconfig.h src/httputils.h src/httpserver.h src/webservercallback.h src/webserverrecallback.h src/webserver.h
                                    src/errordelegate.h src/mimetypes.h )
include_directories(${ROOT}/src ${ASIO_INCLUDE_DIR} ${RE2_INCLUDE_DIR} ${OPENSSL_INCLUDES})
add_library(${PROJECT_NAME} ${HTTP_SRC_LIST})
target_link_libraries(${PROJECT_NAME} re ${LIBS} ${CMAKE_THREAD_LIBS_INIT})

add_executable( httpserver main.cpp )
target_link_libraries( httpserver ${PROJECT_NAME} re ${LIBS} ${CMAKE_THREAD_LIBS_INIT} )

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
   target_link_libraries(testmain_lightning ${GMOCK_MAIN_PATH} ${PROJECT_NAME} re gtest ${LIBS})
   add_test(lightning-tests testmain_lightning)
endif()

# -------------------------------------------------------------------------
# -----------------------   Doxygen                 -----------------------
# -------------------------------------------------------------------------
find_package(Doxygen)
if(DOXYGEN_FOUND)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
add_custom_target(doc ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen" VERBATIM
)
endif(DOXYGEN_FOUND)
