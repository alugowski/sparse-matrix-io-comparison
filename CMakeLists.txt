cmake_minimum_required(VERSION 3.11 FATAL_ERROR)
project(sparse_matrix_io_comparison VERSION 1.0.0 LANGUAGES CXX)

include(FetchContent)

# Add Google Benchmark
include(cmake/GoogleBenchmark.cmake)

# Generator (uses fast_matrix_market)
include(cmake/fast_matrix_market.cmake)
add_executable(generate_matrix_market generate_matrix_market.cpp)
target_link_libraries(generate_matrix_market fast_matrix_market::fast_matrix_market)

# fast_matrix_market
add_executable(fmm bench_main.cpp fmm.cpp bench_common.hpp)
target_link_libraries(fmm benchmark::benchmark fast_matrix_market::fast_matrix_market)

# PIGO
include(cmake/PIGO.cmake)
add_executable(pigo_bench bench_main.cpp pigo_bench.cpp bench_common.hpp)
target_link_libraries(pigo_bench benchmark::benchmark fast_matrix_market::fast_matrix_market pigo)
