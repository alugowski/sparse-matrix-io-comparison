// Copyright (C) 2023 Adam Lugowski. All rights reserved.
// Use of this source code is governed by the BSD 2-clause license found in the LICENSE.txt file.
// SPDX-License-Identifier: BSD-2-Clause

#include "common.hpp"
#include <Eigen/Sparse>

#include <fast_matrix_market/app/Eigen.hpp>

typedef Eigen::SparseMatrix<VALUE_TYPE> SpMat;

/**
 * Read MatrixMarket with Eigen.
 */
void eigen_read_FMM(benchmark::State& state) {
    problem& prob = get_problem((int)state.range(0));

    std::size_t num_bytes = 0;

    for ([[maybe_unused]] auto _ : state) {
        SpMat A;

        std::ifstream f(prob.mm_path);
        fast_matrix_market::read_matrix_market_eigen(f, A);

        num_bytes += std::filesystem::file_size(prob.mm_path);
        benchmark::ClobberMemory();
    }

    state.SetBytesProcessed((int64_t)num_bytes);
    state.SetLabel("problem_name=" + prob.name);
}

BENCHMARK(eigen_read_FMM)->Name("op:read/impl:Eigen_FMM/format:MatrixMarket")->UseRealTime()->Iterations(num_iterations)->Apply(BenchmarkArgument);

/**
 * Write MatrixMarket with Eigen.
 */
void eigen_write_FMM(benchmark::State& state) {
    std::size_t num_bytes = 0;

    problem& prob = get_problem((int)state.range(0));

    fast_matrix_market::write_options options;
    options.parallel_ok = true;
    options.num_threads = (int)state.range(1);

    // load the problem to be written later
    SpMat A;
    {
        std::ifstream f(prob.mm_path);
        fast_matrix_market::read_matrix_market_eigen(f, A);
    }

    auto out_path = temporary_write_dir / ("write_" + prob.name + ".mtx");

    for ([[maybe_unused]] auto _ : state) {
        std::ofstream f{out_path, std::ios_base::binary};
        fast_matrix_market::write_matrix_market_eigen(f, A);

        num_bytes += std::filesystem::file_size(out_path);
        benchmark::ClobberMemory();
    }

    if (delete_written_files_on_finish) {
        std::filesystem::remove(out_path);
    }
    state.SetBytesProcessed((int64_t)num_bytes);
    state.SetLabel("problem_name=" + prob.name);
}

BENCHMARK(eigen_write_FMM)->Name("op:write/impl:Eigen_FMM/format:MatrixMarket")->UseRealTime()->Iterations(num_iterations)->Apply(BenchmarkArgument);
