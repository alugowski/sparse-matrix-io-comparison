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
        triplet_matrix<INDEX_TYPE, VALUE_TYPE> triplet;

        std::ifstream iss(prob.mm_path);
        fast_matrix_market::read_matrix_market_triplet(iss, header, triplet.rows, triplet.cols, triplet.vals, options);
        num_bytes += std::filesystem::file_size(prob.mm_path);
        benchmark::ClobberMemory();
    }

    state.SetBytesProcessed((int64_t)num_bytes);
    state.SetLabel("problem_name=" + prob.name);
}

BENCHMARK(FMM_read)->Name("op:read/impl:FMM/format:MatrixMarket")->UseRealTime()->Iterations(num_iterations)->Apply(BenchmarkArgument);

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
    triplet_matrix<INDEX_TYPE, VALUE_TYPE> triplet;
    {
        std::ifstream f(prob.mm_path);
        fast_matrix_market::read_matrix_market_triplet(f, triplet.nrows, triplet.ncols, triplet.rows, triplet.cols, triplet.vals);
    }

    auto out_path = temporary_write_dir / ("write_" + prob.name + ".mtx");

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

    if (delete_written_files_on_finish) {
        std::filesystem::remove(out_path);
    }
    state.SetBytesProcessed((int64_t)num_bytes);
    state.SetLabel("problem_name=" + prob.name);
}

BENCHMARK(FMM_write)->Name("op:write/impl:FMM/format:MatrixMarket")->UseRealTime()->Iterations(num_iterations)->Apply(BenchmarkArgument);

/**
 * Write MatrixMarket with fast_matrix_market.
 */
void FMM_write_pattern(benchmark::State& state) {
    std::size_t num_bytes = 0;

    problem& prob = get_problem((int)state.range(0));

    fast_matrix_market::write_options options;
    options.parallel_ok = true;
    options.num_threads = (int)state.range(1);

    // load the problem to be written later
    triplet_matrix<INDEX_TYPE, VALUE_TYPE> triplet;
    {
        std::ifstream f(prob.mm_path);
        fast_matrix_market::read_matrix_market_triplet(f, triplet.nrows, triplet.ncols, triplet.rows, triplet.cols, triplet.vals);

        // do not care about the values
        triplet.vals.clear();
    }

    auto out_path = temporary_write_dir / ("write_" + prob.name + "-pattern.mtx");

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

    if (delete_written_files_on_finish) {
        std::filesystem::remove(out_path);
    }
    state.SetBytesProcessed((int64_t)num_bytes);
    state.SetLabel("problem_name=" + prob.name);
}

BENCHMARK(FMM_write_pattern)->Name("op:write/impl:FMM/format:MatrixMarket(pattern)")->UseRealTime()->Iterations(num_iterations)->Apply(BenchmarkArgument);
