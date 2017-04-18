# -------------------------------------------------------------------------
# Build lightning documentation
# -------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.0)

project( benchmark )

# -------------------------------------------------------------------------
# -----------------------        Benchmark          -----------------------
# -------------------------------------------------------------------------
include( ../cmake/external/nonius.cmake )
include( ../cmake/external/weighttp.cmake )

#TODO BUILD OPTIMIZED
add_executable(abc ${CMAKE_CURRENT_SOURCE_DIR}/ab.c)
target_link_libraries(abc -pthread )

include_directories( ${LIGHTNING_SOURCES} ${INCLUDES} ${NONIUS_INCLUDE_DIR} )
SET(BENCHMARK_LIST benchmark.cpp  )
add_executable(benchmark_main ${BENCHMARK_LIST} )
target_compile_options( benchmark_main PRIVATE -O3 )
target_link_libraries(benchmark_main re2 nonius asio weighttp ${LIBS} )

add_custom_target(bench
    ${CMAKE_CURRENT_BINARY_DIR}/benchmark_main -r html -o ${CMAKE_CURRENT_BINARY_DIR}/../doc/benchmark.html -t "lightning benchmarks" -s 250
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Run the benchmarks and create charts." VERBATIM
)
add_custom_target(abc_run ${CMAKE_COMMAND} -E env PATH=$PATH:${WEIGHTTP_PATH} ${CMAKE_BINARY_DIR}/abc [0-100:1k+10x3] 192.168.0.17:9999/index.html
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Run the server benchmarks and create charts." VERBATIM
)
