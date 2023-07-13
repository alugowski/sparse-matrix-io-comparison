// Copyright (C) 2023 Adam Lugowski. All rights reserved.
// Use of this source code is governed by the BSD 2-clause license found in the LICENSE.txt file.
// SPDX-License-Identifier: BSD-2-Clause

#include "common.hpp"
#include <Eigen/Sparse>
#include <unsupported/Eigen/SparseExtra>

#include <fast_matrix_market/app/Eigen.hpp>

typedef Eigen::SparseMatrix<VALUE_TYPE> SpMat;

/**
 * Read MatrixMarket with Eigen.
 */
void eigen_read(benchmark::State& state) {
    problem& prob = get_problem((int)state.range(0));

    std::size_t num_bytes = 0;

    for ([[maybe_unused]] auto _ : state) {
        SpMat A;
        Eigen::loadMarket(A, prob.mm_path);

        num_bytes += std::filesystem::file_size(prob.mm_path);
        benchmark::ClobberMemory();
    }

    state.SetBytesProcessed((int64_t)num_bytes);
    state.SetLabel("problem_name=" + prob.name);
}

BENCHMARK(eigen_read)->Name("op:read/impl:Eigen/format:MatrixMarket")->UseRealTime()->Iterations(num_iterations)->Apply(BenchmarkArgument);

/**
 * Write MatrixMarket with Eigen.
 */
void eigen_write(benchmark::State& state) {
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
        Eigen::saveMarket(A, out_path);

        num_bytes += std::filesystem::file_size(out_path);
        benchmark::ClobberMemory();
    }

    if (delete_written_files_on_finish) {
        std::filesystem::remove(out_path);
    }
    state.SetBytesProcessed((int64_t)num_bytes);
    state.SetLabel("problem_name=" + prob.name);
}

BENCHMARK(eigen_write)->Name("op:write/impl:Eigen/format:MatrixMarket")->UseRealTime()->Iterations(num_iterations)->Apply(BenchmarkArgument);
