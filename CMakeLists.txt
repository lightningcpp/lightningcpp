# -------------------------------------------------------------------------
# Build lightning Project
# -------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.3)

project(lightning)

include(CTest)

SET(MAJOR_VERSION "0")
SET(MINOR_VERSION "1")
SET(PATCH_VERSION "1" CACHE PATCH_VERSION "The package build number." )

# -------------------------------------------------------------------------
# -----------------------   Options and Parameters  -----------------------
# -------------------------------------------------------------------------
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)
SET(LIGHTNING_TAG_VERSION "master" CACHE STRING "The tagged version.")
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

# -----------------------        Build Targets          -----------------------
if ( build_example_server OR build_tests OR build_benchmark )

    #add headers for editor
    FILE(GLOB_RECURSE HeaderFiles "${LIGHTNING_SOURCES}/*.h")
    add_custom_target(headers SOURCES ${HeaderFiles})

    set (CMAKE_CXX_STANDARD 14)
    set (CMAKE_CXX_STANDARD_REQUIRED ON)

    set (THREADS_PREFER_PTHREAD_FLAG ON)
    find_package (Threads REQUIRED)

    find_program(CCACHE_FOUND ccache)
    if(CCACHE_FOUND)
        set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
        set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
    endif(CCACHE_FOUND)

    # ---------------------   External Dependencies       ---------------------
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup()

    # -----------------------      Compile Code         -----------------------
    if( build_example_server )
        add_subdirectory ( sample )
    endif()

    # -----------------------        Test Code          -----------------------
    if (build_tests)
        add_subdirectory ( test )
    endif()

    # ------------------           Benchmark               --------------------
    if (build_benchmark)
        add_subdirectory ( benchmark )
    endif(build_benchmark)
endif()

# -----------------------   Doxygen                 -----------------------
if (build_documentation)
    add_custom_target(cppcheck /usr/bin/cppcheck -DHTTP_BUFFER_SIZE=${HTTP_BUFFER_SIZE} -DHTTP_CLIENT_TIMEOUT=${HTTP_CLIENT_TIMEOUT} -DHTTP_SERVER_THREAD_POOL_SIZE=${HTTP_SERVER_THREAD_POOL_SIZE} --enable=all --inconclusive --xml --xml-version=2 ${HeaderFiles} 2> cppcheck.xml
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating code analysis with CppCheck" VERBATIM
    )
    add_subdirectory ( doc )
endif(build_documentation)

# -----------------------   Testimage for Docker                 -----------------------
IF(EXISTS "${CMAKE_ROOT}/Modules/CPack.cmake")
install(DIRECTORY ${PROJECT_SOURCE_DIR}/test/files DESTINATION share/ )
# TODO install(DIRECTORY ${PROJECT_BINARY_DIR}/doc/docs DESTINATION share/ )

INCLUDE(InstallRequiredSystemLibraries)
SET(CPACK_PACKAGING_INSTALL_PREFIX "/usr/local")
SET(CPACK_GENERATOR "TGZ")
SET(CPACK_COMPONENTS_ALL_IN_ONE_PACKAGE 1)
SET(CPACK_INCLUDE_TOPLEVEL_DIRECTORY 0)

SET(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}_${LIGHTNING_TAG_VERSION}")
SET(CPACK_SOURCE_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}_${LIGHTNING_TAG_VERSION}")
INCLUDE(CPack)

ENDIF(EXISTS "${CMAKE_ROOT}/Modules/CPack.cmake")
