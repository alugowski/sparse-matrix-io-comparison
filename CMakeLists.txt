cmake_minimum_required(VERSION 3.11 FATAL_ERROR)
project(sparse_matrix_io_comparison VERSION 1.0.0 LANGUAGES CXX)

include(FetchContent)

# Add Google Benchmark
include(cmake/GoogleBenchmark.cmake)

# Add fast_matrix_market, used for utilities as well as benchmarks
include(cmake/fast_matrix_market.cmake)

# Generator (uses fast_matrix_market)
add_executable(generate_matrix_market generate_matrix_market.cpp)
target_link_libraries(generate_matrix_market fast_matrix_market::fast_matrix_market)

# Sorts matrix indices (uses fast_matrix_market)
add_executable(sort_matrix_market sort_matrix_market.cpp)
target_link_libraries(sort_matrix_market fast_matrix_market::fast_matrix_market)

# fast_matrix_market benchmark
add_executable(bench_fmm main.cpp bench_fmm.cpp common.hpp)
target_link_libraries(bench_fmm benchmark::benchmark fast_matrix_market::fast_matrix_market)

# PIGO benchmark
include(cmake/PIGO.cmake)
add_executable(bench_pigo main.cpp bench_pigo.cpp common.hpp)
target_link_libraries(bench_pigo benchmark::benchmark fast_matrix_market::fast_matrix_market pigo)

# Eigen benchmark
include(cmake/Eigen.cmake)
find_package (Eigen3 3.4 REQUIRED NO_MODULE)
add_executable(bench_eigen main.cpp bench_eigen.cpp common.hpp)
target_link_libraries(bench_eigen benchmark::benchmark fast_matrix_market::fast_matrix_market Eigen3::Eigen)

add_executable(bench_eigen_fmm main.cpp bench_eigen_fmm.cpp common.hpp)
target_link_libraries(bench_eigen_fmm benchmark::benchmark fast_matrix_market::fast_matrix_market Eigen3::Eigen)

# GraphBLAS
include(cmake/GraphBLAS.cmake)

if (GraphBLAS_FOUND)
    message("GraphBLAS found.")
    message("GRAPHBLAS_INCLUDE_DIR: ${GRAPHBLAS_INCLUDE_DIR}")
    message("GRAPHBLAS_LIBRARIES: ${GRAPHBLAS_LIBRARIES}")
    message("GRAPHBLAS_STATIC: ${GRAPHBLAS_STATIC}")
    message("GRAPHBLAS_LIBRARY: ${GRAPHBLAS_LIBRARY}")

    # GraphBLAS fast_matrix_market bindings benchmark
    add_executable(bench_graphblas_fmm main.cpp bench_graphblas_fmm.cpp common.hpp)
    if (NOT ("${GRAPHBLAS_INCLUDE_DIR}" STREQUAL "" ))
        target_include_directories(bench_graphblas_fmm PUBLIC ${GRAPHBLAS_INCLUDE_DIR})
    endif()
    target_link_libraries(bench_graphblas_fmm benchmark::benchmark fast_matrix_market::fast_matrix_market ${GRAPHBLAS_LIBRARIES})


    # LAGraph
    if (EXISTS "${CMAKE_SOURCE_DIR}/lagraph_lib/LAGraph/build")
        message("Found LAGraph, configuring bench_lagraph")

        add_executable(bench_lagraph main.cpp bench_lagraph.cpp common.hpp)
        if (NOT ("${GRAPHBLAS_INCLUDE_DIR}" STREQUAL "" ))
            target_include_directories(bench_lagraph PUBLIC ${GRAPHBLAS_INCLUDE_DIR})
        endif()
        target_link_directories(bench_lagraph PUBLIC "${CMAKE_SOURCE_DIR}/lagraph_lib/LAGraph/build")
        target_include_directories(bench_lagraph PUBLIC "${CMAKE_SOURCE_DIR}/lagraph_lib/LAGraph/include")
        target_link_libraries(bench_lagraph benchmark::benchmark fast_matrix_market::fast_matrix_market ${GRAPHBLAS_LIBRARIES} "lagraph")
    else()
        message("Skipping bench_lagraph because LAGraph cannot be fetched automatically by CMake.")
        message("Please run lagraph_lib/get_lagraph.sh to build LAGraph, then rerun cmake to build bench_lagraph.")
    endif()
else()
    message("GraphBLAS not found, skipping GraphBLAS benchmarks.")
endif()
