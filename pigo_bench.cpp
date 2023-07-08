// Copyright (C) 2023 Adam Lugowski. All rights reserved.
// Use of this source code is governed by the BSD 2-clause license found in the LICENSE.txt file.
// SPDX-License-Identifier: BSD-2-Clause

#include "bench_common.hpp"

#include "pigo.hpp"

// PIGO memory maps the input file. Multiple iterations
// can mean that the second and following iterations are on a warm cache.
// Both cold and warm caches are valid benchmark options, but default to cold.
//#define PIGO_iterations num_iterations
#define PIGO_iterations 1

/**
 * Read MatrixMarket with PIGO.
 */
static void PIGO_read(benchmark::State& state) {
    problem& prob = get_problem((int)state.range(0));
    int num_threads = (int)state.range(1);
    omp_set_num_threads(num_threads);

    std::size_t num_bytes = 0;

    for ([[maybe_unused]] auto _ : state) {
        pigo::COO<> c {prob.mm_path};
        benchmark::DoNotOptimize(c);

        num_bytes += std::filesystem::file_size(prob.mm_path);
        benchmark::ClobberMemory();
    }

    state.SetBytesProcessed((int64_t)num_bytes);
    state.SetLabel("problem_name=" + prob.name);
}

BENCHMARK(PIGO_read)->Name("op:read/matrix:Coordinate/format:MatrixMarket/impl:PIGO/lang:C++")->UseRealTime()->Iterations(PIGO_iterations)->Apply(BenchmarkArgument);

/**
 * Write an ASCII file with PIGO.
 */
static void PIGO_write_binary(benchmark::State& state) {
    std::size_t num_bytes = 0;

    problem& prob = get_problem((int)state.range(0));
    int num_threads = (int)state.range(1);

    // load the problem to be written later
    omp_set_num_threads(0);
    pigo::COO<> c {prob.mm_path};

    omp_set_num_threads(num_threads);
    auto out_path = std::filesystem::temp_directory_path() / ("write_" + prob.name + ".bin");

    for ([[maybe_unused]] auto _ : state) {
        c.save(out_path);
        num_bytes += std::filesystem::file_size(out_path);
        benchmark::ClobberMemory();
    }

    std::filesystem::remove(out_path);

    state.SetBytesProcessed((int64_t)num_bytes);
    state.SetLabel("problem_name=" + prob.name);
}

BENCHMARK(PIGO_write_binary)->Name("op:write/matrix:Coordinate/format:binary/impl:PIGO/lang:C++")->UseRealTime()->Iterations(PIGO_iterations)->Apply(BenchmarkArgument);

/**
 * Write an ASCII file with PIGO.
 */
static void PIGO_write_ascii(benchmark::State& state) {
    std::size_t num_bytes = 0;

    problem& prob = get_problem((int)state.range(0));
    int num_threads = (int)state.range(1);

    // load the problem to be written later
    omp_set_num_threads(0);
    pigo::COO<> c {prob.mm_path};

    omp_set_num_threads(num_threads);
    auto out_path = std::filesystem::temp_directory_path() / ("write_" + prob.name + ".txt");

    for ([[maybe_unused]] auto _ : state) {
        c.write(out_path);
        num_bytes += std::filesystem::file_size(out_path);
        benchmark::ClobberMemory();
    }

    std::filesystem::remove(out_path);

    state.SetBytesProcessed((int64_t)num_bytes);
    state.SetLabel("problem_name=" + prob.name);
}

BENCHMARK(PIGO_write_ascii)->Name("op:write/matrix:Coordinate/format:ASCII/impl:PIGO/lang:C++")->UseRealTime()->Iterations(PIGO_iterations)->Apply(BenchmarkArgument);
