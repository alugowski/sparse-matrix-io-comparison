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

// PIGO's types
// User warning! pigo::COO is unweighted by default!
using pigo_COO = pigo::COO<
    INDEX_TYPE,  // class Label=uint32_t,
    INDEX_TYPE,  // class Ordinal=Label,
    INDEX_TYPE*, // class Storage=Label*,
    false,       // bool symmetric=false,
    false,       // bool keep_upper_triangle_only=false,
    false,       // bool remove_self_loops=false,
    true,        // bool weighted=false,
    VALUE_TYPE   // class Weight=float,
>;

/**
 * Read MatrixMarket with PIGO.
 */
static void PIGO_read(benchmark::State& state) {
    problem& prob = get_problem((int)state.range(0));
    int num_threads = (int)state.range(1);
    omp_set_num_threads(num_threads);

    std::size_t num_bytes = 0;

    for ([[maybe_unused]] auto _ : state) {
        pigo_COO c {prob.mm_path};
        benchmark::DoNotOptimize(c);

        num_bytes += std::filesystem::file_size(prob.mm_path);
        benchmark::ClobberMemory();
    }

    state.SetBytesProcessed((int64_t)num_bytes);
    state.SetLabel("problem_name=" + prob.name);
}

BENCHMARK(PIGO_read)->Name("op:read/impl:PIGO/format:MatrixMarket")->UseRealTime()->Iterations(PIGO_iterations)->Apply(BenchmarkArgument);

/**
 * Write an ASCII file with PIGO.
 */
static void PIGO_write_binary(benchmark::State& state) {
    std::size_t num_bytes = 0;

    problem& prob = get_problem((int)state.range(0));
    int num_threads = (int)state.range(1);

    // load the problem to be written later
    omp_set_num_threads(0);
    pigo_COO c {prob.mm_path};

    omp_set_num_threads(num_threads);
    auto out_path = temporary_write_dir / ("write_" + prob.name + ".bin");

    for ([[maybe_unused]] auto _ : state) {
        c.save(out_path);
        num_bytes += std::filesystem::file_size(out_path);
        benchmark::ClobberMemory();
    }

    if (delete_written_files_on_finish) {
        std::filesystem::remove(out_path);
    }

    state.SetBytesProcessed((int64_t)num_bytes);
    state.SetLabel("problem_name=" + prob.name);
}

BENCHMARK(PIGO_write_binary)->Name("op:write/impl:PIGO/format:binary")->UseRealTime()->Iterations(PIGO_iterations)->Apply(BenchmarkArgument);

/**
 * Write an ASCII file with PIGO.
 */
static void PIGO_write_ascii(benchmark::State& state) {
    std::size_t num_bytes = 0;

    problem& prob = get_problem((int)state.range(0));
    int num_threads = (int)state.range(1);

    // load the problem to be written later
    omp_set_num_threads(0);
    pigo_COO c {prob.mm_path};

    omp_set_num_threads(num_threads);
    auto out_path = temporary_write_dir / ("write_" + prob.name + ".txt");

    for ([[maybe_unused]] auto _ : state) {
        c.write(out_path);
        num_bytes += std::filesystem::file_size(out_path);
        benchmark::ClobberMemory();
    }

    if (delete_written_files_on_finish) {
        std::filesystem::remove(out_path);
    }

    state.SetBytesProcessed((int64_t)num_bytes);
    state.SetLabel("problem_name=" + prob.name);
}

// Disabled by default
// pigo::COO::write uses std::to_string to write values. This method does not paralellize, so this
// benchmark is very slow on large datasets.
// BENCHMARK(PIGO_write_ascii)->Name("op:write/impl:PIGO/format:ASCII")->UseRealTime()->Iterations(PIGO_iterations)->Apply(BenchmarkArgument);
