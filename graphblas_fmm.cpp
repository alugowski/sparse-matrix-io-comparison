// Copyright (C) 2023 Adam Lugowski. All rights reserved.
// Use of this source code is governed by the BSD 2-clause license found in the LICENSE.txt file.
// SPDX-License-Identifier: BSD-2-Clause

#include "bench_common.hpp"
#include <fast_matrix_market/app/GraphBLAS.hpp>

/**
 * Initialize and finalize GraphBLAS using a global so the rest of the code doesn't have to worry about it.
 * GraphBLAS needs GrB_init() to be called before any other methods, else you get a GrB_PANIC.
 */
struct GraphBLASInitializer {
    GraphBLASInitializer() {
        GrB_init(GrB_BLOCKING);
    }

    ~GraphBLASInitializer() {
        GrB_finalize();
    }
};
[[maybe_unused]] GraphBLASInitializer graphblas_init_and_finalizer{};

/**
 * Read MatrixMarket with fast_matrix_market.
 */
void GraphBLAS_read_FMM(benchmark::State& state) {
    problem& prob = get_problem((int)state.range(0));

    // read options
    fast_matrix_market::read_options options{};
    options.parallel_ok = true;
    options.num_threads = (int)state.range(1);

    std::size_t num_bytes = 0;

    for ([[maybe_unused]] auto _ : state) {
        std::ifstream iss(prob.mm_path);

        GrB_Matrix mat;
        fast_matrix_market::read_matrix_market_graphblas(iss, &mat, options);
        GrB_Matrix_free(&mat);

        num_bytes += std::filesystem::file_size(prob.mm_path);
        benchmark::ClobberMemory();
    }

    state.SetBytesProcessed((int64_t)num_bytes);
    state.SetLabel("problem_name=" + prob.name);
}

BENCHMARK(GraphBLAS_read_FMM)->Name("op:read/impl:GraphBLAS_FMM/format:MatrixMarket")->UseRealTime()->Iterations(num_iterations)->Apply(BenchmarkArgument);

/**
 * Write MatrixMarket with fast_matrix_market.
 */
void GraphBLAS_write_FMM(benchmark::State& state) {
    std::size_t num_bytes = 0;

    problem& prob = get_problem((int)state.range(0));

    fast_matrix_market::write_options options;
    options.parallel_ok = true;
    options.num_threads = (int)state.range(1);

    // load the problem to be written later
    GrB_Matrix mat;
    {
        std::ifstream f(prob.mm_path);
        fast_matrix_market::read_matrix_market_graphblas(f, &mat);
    }

    auto out_path = temporary_write_dir / ("write_" + prob.name + ".mtx");

    for ([[maybe_unused]] auto _ : state) {
        std::ofstream oss{out_path, std::ios_base::binary};

        fast_matrix_market::write_matrix_market_graphblas(oss, mat, options);

        num_bytes += std::filesystem::file_size(out_path);
        benchmark::ClobberMemory();
    }
    GrB_Matrix_free(&mat);

    if (delete_written_files_on_finish) {
        std::filesystem::remove(out_path);
    }
    state.SetBytesProcessed((int64_t)num_bytes);
    state.SetLabel("problem_name=" + prob.name);
}

BENCHMARK(GraphBLAS_write_FMM)->Name("op:write/impl:GraphBLAS_FMM/format:MatrixMarket")->UseRealTime()->Iterations(num_iterations)->Apply(BenchmarkArgument);
