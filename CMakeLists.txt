# -------------------------------------------------------------------------
# Build lightning Project
# -------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.5.0)

project(lightning)

SET(MAJOR_VERSION "0")
SET(MINOR_VERSION "1")
SET(PATCH_VERSION "0" CACHE PATCH_VERSION "The package build number." )

# -------------------------------------------------------------------------
# -----------------------   Options and Parameters  -----------------------
# -------------------------------------------------------------------------
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)
option(build_example_server "Build the example server." OFF )
option(build_tests "Build all http unit tests." OFF )
option(build_documentation "Build the documentation." OFF )
option(build_benchmark "Build the benchmarks and execute." OFF )
SET(HTTP_BUFFER_SIZE "8192" CACHE STRING "The HTTP character buffer size.")
SET(HTTP_CLIENT_TIMEOUT "3" CACHE STRING "The HTTP connection timeout in seconds.")
SET(HTTP_SERVER_THREAD_POOL_SIZE "10" CACHE STRING "The number of threads created on the ASIO service.")
SET(TESTFILES "${PROJECT_SOURCE_DIR}/test/files" CACHE PATH "The path to the testfiles.")
SET(DOCFILES "${PROJECT_BINARY_DIR}/doc/" CACHE PATH "The path to the documentation files.")
SET(LIGHTNING_SOURCES "${PROJECT_SOURCE_DIR}/include/" )
add_definitions( -DHTTP_BUFFER_SIZE=${HTTP_BUFFER_SIZE} -DHTTP_CLIENT_TIMEOUT=${HTTP_CLIENT_TIMEOUT}
                 -DHTTP_SERVER_THREAD_POOL_SIZE=${HTTP_SERVER_THREAD_POOL_SIZE}
                 -DTESTFILES="${TESTFILES}/" -DDOCFILES="${DOCFILES}/"
                 -DASIO_STANDALONE -DHTTP_CPP_RELEASE="${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}" )

if ( build_example_server OR build_tests OR build_benchmark )

    #add headers for editor
    FILE(GLOB_RECURSE HeaderFiles "${LIGHTNING_SOURCES}/*.h")
    add_custom_target(headers SOURCES ${HeaderFiles})

    if(CMAKE_COMPILER_IS_GNUCXX)
        add_definitions(-std=c++14)
    endif()

    # -------------------------------------------------------------------------
    # ---------------------   External Dependencies       ---------------------
    # -------------------------------------------------------------------------

    include( ExternalProject )
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
    find_package(Boost REQUIRED system timer chrono)

    SET(LIBS ${LIBS} ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES}
             -lssl -lcrypto -luuid -lpthread -lm ) #needed for ubuntu

    SET( INCLUDES ${INCLUDES} ${Boost_INCLUDE_DIRS} ${OPENSSL_INCLUDES} )

    # -------------------------------------------------------------------------
    # -----------------------      Compile Code         -----------------------
    # -------------------------------------------------------------------------
    if( build_example_server )
        add_subdirectory ( sample )
    endif()

    # -------------------------------------------------------------------------
    # -----------------------        Test Code          -----------------------
    # -------------------------------------------------------------------------
    if (build_tests)
        enable_testing()
        add_subdirectory ( test )
    endif()

    # -------------------------------------------------------------------------
    # ------------------           Benchmark               --------------------
    # -------------------------------------------------------------------------
    if (build_benchmark)
        add_subdirectory ( benchmark )
    endif(build_benchmark)


endif()

# -------------------------------------------------------------------------
# -----------------------   Doxygen                 -----------------------
# -------------------------------------------------------------------------

if (build_documentation)
    add_custom_target(cppcheck /usr/bin/cppcheck --enable=all --inconclusive --xml --xml-version=2 ${HeaderFiles} 2> cppcheck.xml
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating code analysis with CppCheck" VERBATIM
    )
    add_subdirectory ( doc )
endif(build_documentation)
