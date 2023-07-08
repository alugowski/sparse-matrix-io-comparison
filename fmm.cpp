// Copyright (C) 2023 Adam Lugowski. All rights reserved.
// Use of this source code is governed by the BSD 2-clause license found in the LICENSE.txt file.
// SPDX-License-Identifier: BSD-2-Clause

#include "bench_common.hpp"
#include <fast_matrix_market/fast_matrix_market.hpp>

template <typename IT, typename VT>
struct triplet_matrix {
    int64_t nrows = 0, ncols = 0;
    std::vector<IT> rows;
    std::vector<IT> cols;
    std::vector<VT> vals;

    [[nodiscard]] size_t size_bytes() const {
        return sizeof(IT)*rows.size() + sizeof(IT)*cols.size() + sizeof(VT)*vals.size();
    }
};

template <typename IT, typename VT>
struct csc_matrix {
    int64_t nrows = 0, ncols = 0;
    std::vector<IT> indptr;
    std::vector<IT> indices;
    std::vector<VT> vals;

    [[nodiscard]] size_t size_bytes() const {
        return sizeof(IT)*indptr.size() + sizeof(IT)*indices.size() + sizeof(VT)*vals.size();
    }
};

template <typename VT>
struct array_matrix {
    int64_t nrows = 0, ncols = 0;
    std::vector<VT> vals;

    [[nodiscard]] size_t size_bytes() const {
        return sizeof(VT)*vals.size();
    }
};

/**
 * Read MatrixMarket with fast_matrix_market.
 */
void FMM_read(benchmark::State& state) {
    problem& prob = get_problem((int)state.range(0));

    // read options
    fast_matrix_market::read_options options{};
    options.parallel_ok = true;
    options.num_threads = (int)state.range(1);

    std::size_t num_bytes = 0;

    for ([[maybe_unused]] auto _ : state) {
        fast_matrix_market::matrix_market_header header;
        triplet_matrix<int32_t, VT> triplet;

        std::ifstream iss(prob.mm_path);
        fast_matrix_market::read_matrix_market_triplet(iss, header, triplet.rows, triplet.cols, triplet.vals, options);
        num_bytes += std::filesystem::file_size(prob.mm_path);
        benchmark::ClobberMemory();
    }

    state.SetBytesProcessed((int64_t)num_bytes);
    state.SetLabel("problem_name=" + prob.name);
}

BENCHMARK(FMM_read)->Name("op:read/matrix:Coordinate/format:MatrixMarket/impl:FMM/lang:C++")->UseRealTime()->Iterations(num_iterations)->Apply(BenchmarkArgument);

/**
 * Write MatrixMarket with fast_matrix_market.
 */
void FMM_write(benchmark::State& state) {
    std::size_t num_bytes = 0;

    problem& prob = get_problem((int)state.range(0));

    fast_matrix_market::write_options options;
    options.parallel_ok = true;
    options.num_threads = (int)state.range(1);

    // load the problem to be written later
    triplet_matrix<int64_t, VT> triplet;
    {
        std::ifstream f(prob.mm_path);
        fast_matrix_market::read_matrix_market_triplet(f, triplet.nrows, triplet.ncols, triplet.rows, triplet.cols, triplet.vals);
    }

    auto out_path = std::filesystem::temp_directory_path() / ("write_" + prob.name + ".mtx");

    for ([[maybe_unused]] auto _ : state) {
#define USE_OSS 0
#if USE_OSS
        std::ostringstream oss;
#else
        std::ofstream oss{out_path, std::ios_base::binary};
#endif

        fast_matrix_market::write_matrix_market_triplet(oss,
                                                        {triplet.nrows, triplet.ncols},
                                                        triplet.rows, triplet.cols, triplet.vals,
                                                        options);
#if USE_OSS
        num_bytes += oss.str().size();
#else
        num_bytes += std::filesystem::file_size(out_path);
#endif
        benchmark::ClobberMemory();
    }

    std::filesystem::remove(out_path);
    state.SetBytesProcessed((int64_t)num_bytes);
    state.SetLabel("problem_name=" + prob.name);
}

BENCHMARK(FMM_write)->Name("op:write/matrix:Coordinate/format:MatrixMarket/impl:FMM/lang:C++")->UseRealTime()->Iterations(num_iterations)->Apply(BenchmarkArgument);
